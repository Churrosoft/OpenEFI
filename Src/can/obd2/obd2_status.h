/** @file */
// OBD Response Status from ISO15031-5 page 35

#ifndef OBD2_STATUS_H
#define OBD2_STATUS_H

// Negative response:
//  {Response Service Identifier} {Service Identifier} {Response Code}

// Negative Response Codes

/**
 *  This response code indicates that the service is rejected but the server
 *  (ECU) does not specify the reason of the rejection.
 * @see also knows as GR
 */
#define generalReject 0x10
#define GR generalReject

/**
 *  This response code indicates that the requested action will not be taken
 *  because the server (ECU) does not support the requested service.
  * @see also knows as SNS

 */
#define serviceNotSupported 0x11
#define SNS serviceNotSupported

/**
 * This response code indicates that the requested action will not be taken
 * because the server (ECU) does not support the arguments of the request
 * message or the format of the argument bytes do not match the prescribed
 * format for the specified service.
 * @see also knows as SFNSIF
 */
#define subFunctionNotSupported 0x11
#define SFNSIF subFunctionNotSupported

/**
 * This response code indicates that the server (ECU) is temporarily too busy
 * to perform the requested operation.
 * @see also knows as BRR
 * @note If the server (ECU) is able to perform the diagnostic task but needs
 * additional time to finish the task and prepares the response message, \n the
 * negative response message with response code $78 are used instead of $21.
 *
 * @note busy goes brrrrrrrrrr
 */
#define busyRepeatRequest 0x21
#define BRR busyRepeatRequest // BRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR

/**
 * This response code indicates that the requested action will not be taken
 * because the server (ECU) prerequisite conditions are not met. This request
 * may also occur when sequence-sensitive requests are issued in the wrong
 * order.
 * @see also knows as CNCORSE
 */
#define conditionsNotCorrect 0x22
#define CNCORSE conditionsNotCorrect

/**
 * This response code indicates that the request message was received correctly,
 *  and that any parameters in the request message were valid, but the action to
 *  be performed may not be completed yet. This response code can be used to
 *  indicate that the request message was properly received and does not need to
 *  be re-transmitted, but the server (ECU) is not yet ready to receive another
 *  request
 * @see also knows as RCR-RP
 **/
#define requestCorrectResponsePending 0x78
#define RCRRP requestCorrectResponsePending

#endif