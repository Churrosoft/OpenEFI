#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
using nlohmann::json;

struct Name {
  std::string first;
  std::string last;
};

struct Address {
  std::string houseNo;
  std::string street;
  std::string city;
  std::string postalCode;
  std::string country;
};

struct Person {
  Name name;
  Address address;
  int age;
};

struct table_ref {
  uint16_t y_max;
  uint16_t x_max;
  int32_t memory_address;
};

struct injectorConfig {
  float flowCCMin;
  int8_t injectorCount;
  float fuelPressure;
  float fuelDensity;
  float onTime;
  float offTime;
};

struct injectionConfig {
  float targetLambda;    // a cuanto tiene que apuntar la lambda (tendria que ser una tabla)
  float targetStoich;    // proporcion A/F 14.7 nasta (o menos), 9.0 ethanol
  bool enable_alphaN;
  bool enable_speedDensity;
  injectorConfig injector;
  table_ref alphaN_ve_table;
};

struct engine_config {
  bool ready;    // true cuando ya se leyo desde la memoria
  injectionConfig Injection;
};

void to_json(json& j, const Person& p) {
  j = json{{"name", {{"first", p.name.first}, {"last", p.name.last}}},
           {"address",
            {{"house", p.address.houseNo},
             {"street", p.address.street},
             {"city", p.address.city},
             {"postal_code", p.address.postalCode},
             {"country", p.address.country}}},
           {"age", p.age}};
}

void to_json(json& j, const engine_config& p) {
  j = json{
      {"ready", p.ready},
      {"Injection",
       {{"targetLambda", p.Injection.targetLambda},
        {"targetStoich", p.Injection.targetStoich},
        {"enable_alphaN", p.Injection.enable_alphaN},
        {"enable_speedDensity", p.Injection.enable_speedDensity},

        {"injector",
         {
             {"flowCCMin", p.Injection.injector.flowCCMin},
             {"injectorCount", p.Injection.injector.injectorCount},
             {"fuelPressure", p.Injection.injector.fuelPressure},
             {"fuelDensity", p.Injection.injector.fuelDensity},
             {"onTime", p.Injection.injector.onTime},
             {"offTime", p.Injection.injector.offTime},
         }},

        {"alphaN_ve_table",
         {{"x_max", p.Injection.alphaN_ve_table.x_max},
          {"y_max", p.Injection.alphaN_ve_table.y_max},
          {"memory_address", p.Injection.alphaN_ve_table.memory_address}

         }}}},
  };
}

engine_config efi_config;

void set_default() {
  engine_config cfg;
  injectionConfig inj;
  injectorConfig injector;
  inj.targetLambda = 0.98;
  inj.targetStoich = 14.7;
  inj.enable_alphaN = true;

  injector.flowCCMin = 110;
  injector.fuelDensity = 0.726;
  // FIXME: en teoria tendria esto tendria que ser la presion, no el multiplicador sobre la presion base
  injector.fuelPressure = 1;
  // promedio a "ojimetro" de:
  // https://documents.holley.com/techlibrary_terminatorxv2injectordata.pdf
  injector.offTime = 250;
  injector.onTime = 750;

  inj.injector = injector;

  cfg.ready = false;
  cfg.Injection = inj;

  efi_config = cfg;
}

int main() {
  const Person p{{"firstname", "lastname"}, {"123", "St. ABC", "XYZ", "123456", "country"}, 18};

  set_default();

  json jj{efi_config};

  json j{p};
  std::cout << j.dump(4) << '\n';
  std::cout << jj.dump(4) << '\n';

  return 0;
}