#![feature(type_alias_impl_trait)]
#![feature(exclusive_range_pattern)]
#![feature(proc_macro_hygiene)]
#![feature(int_roundings)]
#![feature(is_some_and)]
#![feature(stdsimd)]

#![no_main]
#![no_std]

#![allow(stable_features)]
#![allow(unused_mut)]


use panic_halt as _;
//wip

#[rtic::app(device = stm32f4xx_hal::pac, peripherals = true, dispatchers = [TIM4, TIM7, TIM8_CC])]
mod app {

    use rtic;
    //::app;
    use rtic_monotonics::systick::*;
    use arrayvec::ArrayVec;
    use embedded_hal::spi::{Mode, Phase, Polarity};
    use rtic::Mutex;
    use stm32f4xx_hal::{
        adc::{Adc, config::AdcConfig},
        crc32,
        crc32::Crc32,
        gpio::{Edge, Input},
        otg_fs,
        otg_fs::{USB, UsbBusType},
        pac::{ADC1, TIM13, TIM2, TIM3, TIM5},
        prelude::*,
        spi::*,
        timer::{self, Event},
    };
    use usb_device::{bus::UsbBusAllocator, device::UsbDevice};
    use usbd_serial::SerialPort;
    use usbd_webusb::{url_scheme, WebUsb};

    //use super::*;

    pub mod debug;
    pub mod engine;
    pub mod gpio;
    pub mod gpio_legacy;
    pub mod injection;
    pub mod logging;
    pub mod memory;
    pub mod util;
    pub mod webserial;
    pub mod tasks;

    use crate::app::{
        engine::{
            efi_cfg::{EngineConfig, get_default_efi_cfg},
            engine_status::{EngineStatus, get_default_engine_status},
            sensors::{get_sensor_raw, SensorTypes, SensorValues},
        },
        gpio_legacy::{
            ADCMapping,
            AuxIoMapping,
            IgnitionGpioMapping,
            init_gpio,
            LedGpioMapping,
            InjectionGpioMapping,
            RelayMapping,
            StepperMapping,
        },
        injection::{calculate_time_isr, injection_setup},
        memory::tables::{SpiT, Tables},
        webserial::{handle_tables,SerialMessage, SerialStatus,finish_message},
        // tasks::{engine::ckp_trigger, engine::motor_checks, ignition::ignition_schedule}
    };

    use logging::host;

    #[shared]
    struct Shared {
        led: LedGpioMapping,

        // USB:
        usb_cdc: SerialPort<'static, UsbBusType, [u8; 128], [u8; 4000]>,
        usb_web: WebUsb<UsbBusType>,
    }

    #[local]
    struct Local {
        usb_dev: UsbDevice<'static, UsbBusType>,
        cdc_input_buffer: ArrayVec<u8, 128>,
        state: bool,
        state2: bool,
    }

    #[init(local = [USB_BUS: Option < UsbBusAllocator < UsbBusType >> = None])]
     fn init(cx: init::Context) -> (Shared, Local) {
        // Setup clocks
        //let mut flash = cx.device.FLASH.constrain();
        let mut device = cx.device;
        let mut rcc = device.RCC.constrain();

        // Initialize the systick interrupt & obtain the token to prove that we did
        let systick_mono_token = rtic_monotonics::create_systick_token!();
        Systick::start(cx.core.SYST, 120_000_000, systick_mono_token); // default STM32F407 clock-rate is 36MHz

        let _clocks = rcc
            .cfgr
            .use_hse(25.MHz())
            .sysclk(120.MHz())
            .require_pll48clk()
            .freeze();

        let gpio_a = device.GPIOA.split();
        let gpio_b = device.GPIOB.split();
        let gpio_c = device.GPIOC.split();
        let gpio_d = device.GPIOD.split();
        let gpio_e = device.GPIOE.split();

        let mut gpio_config = init_gpio(gpio_a, gpio_b, gpio_c, gpio_d, gpio_e);

        // ADC
        let mut adc = Adc::adc1(device.ADC1, true, AdcConfig::default());

        adc.enable();
        adc.calibrate();

        // configure CKP/CMP Pin for Interrupts
        let mut ckp = gpio_config.ckp;
        let mut syscfg = device.SYSCFG.constrain();
        host::debug!("init gpio");
        ckp.make_interrupt_source(&mut syscfg);
        ckp.trigger_on_edge(&mut device.EXTI, Edge::Falling);


        // configure the timers

        // timer Tiempo inyeccion
        let mut timer: timer::CounterUs<TIM2> = device.TIM2.counter_us(&_clocks);
        // timer tiempo de ignicion
        let mut timer3: timer::CounterUs<TIM3> = device.TIM3.counter_us(&_clocks);
        // timer CPWM
        let mut timer4: timer::CounterUs<TIM5> = device.TIM5.counter_us(&_clocks);

        // timer uso generico
        let mut timer13: timer::DelayUs<TIM13> = device.TIM13.delay_us(&_clocks);

        //  TIM5, TIM7, TIM4
        host::debug!("init timers");
        timer.start((150).millis()).unwrap();

        // Set up to generate interrupt when timer expires
        timer.listen(Event::Update);
        timer3.listen(Event::Update);
        timer13.listen(Event::Update);
        // timer4.start((70).minutes()).unwrap();
        timer4.start(1_000_000_u32.micros()).unwrap();

        // Init USB
        let usb = USB {
            usb_global: device.OTG_FS_GLOBAL,
            usb_device: device.OTG_FS_DEVICE,
            usb_pwrclk: device.OTG_FS_PWRCLK,
            pin_dm: gpio_config.usb_dp,
            pin_dp: gpio_config.usb_dm,
            hclk: _clocks.hclk(),
        };

        static mut EP_MEMORY: [u32; 1024] = [0; 1024];
        static mut __USB_TX: [u8; 4000] = [0; 4000];
        static mut __USB_RX: [u8; 128] = [0; 128];
        host::debug!("init usb");
        let usb_bus = cx.local.USB_BUS;
        unsafe {
            *usb_bus = Some(otg_fs::UsbBus::new(usb, &mut EP_MEMORY));
        }

        let usb_cdc = unsafe {
            SerialPort::new_with_store(usb_bus.as_ref().unwrap(), __USB_RX, __USB_TX)
        };
        let usb_web = WebUsb::new(
            usb_bus.as_ref().unwrap(),
            url_scheme::HTTPS,
            "tuner.openefi.tech",
        );

        let usb_dev = webserial::new_device(usb_bus.as_ref().unwrap());

        let cdc_buff = ArrayVec::<u8, 128>::new();

        // Schedule the blinking task
        blink::spawn().ok();
        blink2::spawn().ok();

        (Shared { led: gpio_config.led,
            // USB
            usb_cdc,
            usb_web,
        }, Local {
            usb_dev,
            cdc_input_buffer: cdc_buff,
            state: false, state2: false })
    }

    #[task(local = [state], shared = [led])]
    async fn blink(mut cx: blink::Context) {
        loop {
            if *cx.local.state {
                cx.shared.led.lock(|l| l.led_0.set_high());
                *cx.local.state = false;
            } else {
                cx.shared.led.lock(|l| l.led_0.set_low());
                *cx.local.state = true;
            }
            Systick::delay(100.millis()).await;
        }
    }

    #[task(local = [state2], shared = [led])]
    async fn blink2(mut cx: blink2::Context) {
        loop {
            if *cx.local.state2 {
                cx.shared.led.lock(|l| l.led_1.set_high());
                *cx.local.state2 = false;
            } else {
                cx.shared.led.lock(|l| l.led_1.set_low());
                *cx.local.state2 = true;
            }
            Systick::delay(50.millis()).await;
        }
    }

    #[task(shared = [usb_cdc], priority = 2)]
    async fn send_message(
        mut ctx: send_message::Context,
        status: SerialStatus,
        code: u8,
        mut message: SerialMessage,
    ) {
        message.status = status as u8;
        message.code = code;

        ctx.shared.usb_cdc.lock(|cdc| {
            cdc.write(&finish_message(message)).unwrap();
        });
        Systick::delay(50.millis()).await;
    }


    // Externally defined tasks
    extern "Rust" {
        // Low-priority task to send back replies via the serial port. , capacity = 30
        // #[task(shared = [usb_cdc], priority = 2)]
        // async fn send_message(
        //     ctx: send_message::Context,
        //     status: SerialStatus,
        //     code: u8,
        //     mut message: SerialMessage,
        // );
    }
    }