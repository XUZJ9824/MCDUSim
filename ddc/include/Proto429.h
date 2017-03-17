/*********************************************************************/
/*
* This is an Windows 95 specific file
*
* Description:  PCI429 DLL - Dynamic-link library interface from 
*                            Windows 95 to 429 PCI CARD
*
* Modification History:
*
*   07/21/93  RFC
*   20/Mar/95 MGC - Added the function ReadDiscreteQueue
*   06/23/99  KWH - Adapted library to PCI card.
*   04/13/00  KWH - Enhanced Lab Windows support.
*   10/11/00  KWH - Added variable frequency functions:
*                   'SetBitRates()', 'GetBitRates()'.
*/
/*********************************************************************/

/*********************************************************************/
/*
Abstract: All function prototype declarations for the ARINC 429 ToolBox.

      05/05/95 - DFL - Added type defs and defines to make header
              file compatible with the 32-bit LabWindows environment.
*/
/*********************************************************************/
#ifndef _PC_PROTO429
#define _PC_PROTO429

#include "types429.h"
#include "stdwndef.h"

#ifndef FAR
  #define FAR
#endif

#ifdef _NI_i386_
#define CDECL
#define extern
#define far
#define near
#define cdecl
#define huge
#define VOID void
#define GetExtendedScheduledLabelValue Get_Extended_Scheduled_Label_Value
typedef char * LPSTR;
typedef unsigned short WORD;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/************************ Prototype definitions ****************************/
/***************************************************************************/
/*                                                                         */
/* Function Name: AddLabel                                                 */
/*                                                                         */
/* Abstract: Specifies a label that needs to be collected off of the       */
/*           ARINC bus                                                     */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI AddLabel(ChannelType Receiver, LabelType Label);

/***************************************************************************/
/*                                                                         */
/* Function Name: AddLabelGroup                                            */
/*                                                                         */
/* Abstract: Specifies the labels that need to be collected off of the     */
/*           ARINC bus                                                     */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI AddLabelGroup(ChannelType Receiver,
                                   LabelType FAR* Labels, 
                                   USHORT NumOfLabels);

/***************************************************************************/
/*                                                                         */
/* Function Name: CloseChannel                                             */
/*                                                                         */
/* Abstract: Disables a channel.                                           */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI CloseChannel(ChannelType Channel);

/***************************************************************************/
/*                                                                         */
/* Function Name: CloseChannelAll                                          */
/*                                                                         */
/* Abstract: Disables all channels.                                        */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI CloseChannelAll(VOID);

/***************************************************************************/
/*                                                                         */
/* Function Name: DeleteAllLabel                                           */
/*                                                                         */
/* Abstract: Deletes all labels from a receiver channel.                   */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI DeleteAllLabel(ChannelType receiver);

/***************************************************************************/
/*                                                                         */
/* Function Name: DeleteLabel                                              */
/*                                                                         */
/* Abstract: Deletes a label from a receiver channel.                      */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI DeleteLabel(ChannelType receiver, LabelType Label);

/***************************************************************************/
/*                                                                         */
/* Function Name: DeleteLabelGroup                                         */
/*                                                                         */
/* Abstract: Deletes a group of labels from a receiver channel.            */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI DeleteLabelGroup(ChannelType receiver,
                                      LabelType FAR* Labels,
                                      USHORT NumOfLabel);

/***************************************************************************/
/*                                                                         */
/* Function Name: StatusToAscii                                            */
/*                                                                         */
/* Abstract: Converts a STATUS code to ascii.                              */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI StatusToAscii(STATUS Code, LPSTR Str);

/***************************************************************************/
/*                                                                         */
/* Function Name: Initialize                                               */
/*                                                                         */
/* Abstract: Initializes each of the 429 Cards                             */
/*                                                                         */
/* Comments: For each call to 'Initialize()', a call to 'Shutdown()' MUST  */
/*           be made.                                                      */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI Initialize(LPSTR HexFile, 
								LPSTR Unused,
								FARPROC Unused1,
								FARPROC Unused2,
								DiagnosticType Diagnostic,
								ResolutionType TimerResolution,
								CardType Card);

/***************************************************************************/
/*                                                                         */
/* Function Name: Kill_Scheduled                                           */
/*                                                                         */
/* Abstract: Removes a scheduled transmission from a transmitter channel.  */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI Kill_Scheduled(ChannelType transmitter, LabelType Label);

/***************************************************************************/
/*                                                                         */
/* Function Name: Kill_Scheduled_All                                       */
/*                                                                         */
/* Abstract: Removes all scheduled transmissions from a transmitter        */
/*           channel.                                                      */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI Kill_Scheduled_All(ChannelType transmitter);

/*/////////////////////////////////////////////////////////////////////////*/
/*                                                                         */
/* Function Name: Transmit_Extended_Scheduled                              */
/*                                                                         */
/* Abstract: Transmits an ARINC word through channel Transmitter every     */
/*           "UpdateRate" milli seconds.  The transmission is unique based */
/*           on the label and SDI.                                         */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/*/////////////////////////////////////////////////////////////////////////*/
extern STATUS WINAPI Transmit_Extended_Scheduled(
										ChannelType Transmitter,
										USHORT UpdateRate,
										SDI_Type SDI,
										ArincRawWordType FAR* ArincWord);

/*/////////////////////////////////////////////////////////////////////////*/
/*                                                                         */
/* Function Name: Kill_Extended_Scheduled                                  */
/*                                                                         */
/* Abstract: Removes all scheduled transmissions from a transmitter        */
/*           channel.                                                      */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/*/////////////////////////////////////////////////////////////////////////*/
extern STATUS WINAPI Kill_Extended_Scheduled(ChannelType Transmitter,
                                             SDI_Type SDI,
											 LabelType Label);

/*/////////////////////////////////////////////////////////////////////////*/
/*                                                                         */
/* Function Name: Get_Extended_Scheduled_Label_Value                       */
/*                                                                         */
/* Abstract: Gets the status of a label that has previously been setup for */
/*           Transmission specified by label and SDI                       */
/*                                                                         */
/* Comments: Synonymous with 'GetExtendedScheduledLabelValue'              */
/*                                                                         */
/*/////////////////////////////////////////////////////////////////////////*/
extern STATUS WINAPI Get_Extended_Scheduled_Label_Value(
                             ChannelType Transmitter,
                             LabelType Label,
                             SDI_Type SDI,
                             ArincRawWordType FAR* ArincWord,
                             USHORT FAR* UpdateRate);

/***************************************************************************/
/*                                                                         */
/* Function Name: IsBoardPresent                                           */
/*                                                                         */
/* Abstract: Checks to see if a 429 Card is present in a PC                */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/***************************************************************************/
extern PC_BOOLEAN WINAPI IsBoardPresent(ULONG SharedRam);

/***************************************************************************/
/*                                                                         */
/* Function Name: Loader                                                   */
/*                                                                         */
/* Abstract: Loads an Intel executable hex file to a 429 Card.             */
/*                                                                         */
/* Comments: Low level backward compatible call.  Not to be used!          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI Loader(LPSTR load_file, DWORD pc_base, DWORD io_base);

/***************************************************************************/
/*                                                                         */
/* Function Name: OpenChannel                                              */
/*                                                                         */
/* Abstract: Initializes a bus channel.                                    */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI OpenChannel(ChannelType Channel,
								 PC_BOOLEAN Filter,
								 PC_BOOLEAN KeepTimeInfo,
								 PC_BOOLEAN FIFO,
                                 ParityType Parity);

/***************************************************************************/
/*                                                                         */
/* Function Name: GetChannelStatus                                         */
/*                                                                         */
/* Abstract: Gets the current setting for a channel.                       */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI GetChannelStatus( ChannelType Channel,
                                       PC_BOOLEAN FAR* Filter,
                                       PC_BOOLEAN FAR* KeepTimeInfo,
                                       PC_BOOLEAN FAR* FIFO,
                                       ParityType  FAR* Parity,
                                       PC_BOOLEAN Scheduled);

/***************************************************************************/
/*                                                                         */
/* Function Name: GetPendingArincWords                                     */
/*                                                                         */
/* Abstract: Returns the length of channel's queue.                        */
/*                                                                         */
/* Comments:    Value    Meaning                                           */
/*              < 0      Bad argument                                      */
/*              = 0      empty queue                                       */
/*              > 0      number of elem pending in the queue               */
/*                                                                         */
/***************************************************************************/
extern SHORT WINAPI GetPendingArincWords(ChannelType Channel);

/***************************************************************************/
/*                                                                         */
/* Function Name: Panic                                                    */
/*                                                                         */
/* Abstract: ****** Obsolete ******                                        */
/*           Function called when library encounters fatal error.          */
/*                                                                         */
/* Comments: Obsolete.  Nothing is performed if called.                    */
/*                                                                         */
/***************************************************************************/
extern VOID WINAPI Panic(LPSTR Str);

/***************************************************************************/
/*                                                                         */
/* Function Name: Display                                                  */
/*                                                                         */
/* Abstract: ****** Obsolete ******                                        */
/*           Displays a string on STDOUT.                                  */
/*                                                                         */
/* Comments: Obsolete.  Nothing is performed if called.                    */
/*                                                                         */
/***************************************************************************/
extern VOID WINAPI Display(LPSTR Str);

/***************************************************************************/
/*                                                                         */
/* Function Name: SetDisplayFunction                                       */
/*                                                                         */
/* Abstract: ****** Obsolete ******                                        */
/*           Overwrites the default output handling routine. This will     */
/*           enable the application programmer with a hook to define       */
/*           their own output routines.                                    */
/*                                                                         */
/* Comments: Obsolete.  Nothing is performed if called.                    */
/*                                                                         */
/***************************************************************************/
extern VOID WINAPI SetDisplayFunction(FARPROC lpfnDisplayFunction);

/***************************************************************************/
/*                                                                         */
/* Function Name: SetErrorFunction                                         */
/*                                                                         */
/* Abstract: ****** Obsolete ******                                        */
/*           Overwrites the default error handling routine. This will      */
/*           enable the application programmer with a hook to define       */
/*           their own error handling routines.                            */
/*                                                                         */
/* Comments: Obsolete.  Nothing is performed if called.                    */
/*                                                                         */
/***************************************************************************/
extern VOID WINAPI SetErrorFunction(FARPROC lpfnErrorFunction);

/***************************************************************************/
/*                                                                         */
/* Function Name: ReadDiscrete                                             */
/*                                                                         */
/* Abstract: Reads a value from one of the discrete inputs on the Card.    */
/*                                                                         */
/* Comments: None.                                                         */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI ReadDiscrete(DiscreteType DiscreteNum,
                                  DiscreteValue FAR* Value);

/***************************************************************************/
/*                                                                         */
/* Function Name: ReadDiscreteQueue                                        */
/*                                                                         */
/* Abstract: This function returns a status indicating the outcome of the  */
/*           of the attempt to dequeue the values of the discrete inputs   */
/*           from the dual-port ram.  If the value PC_SUCCESS is returned  */
/*           this indicates that the discrete values were successfully     */
/*           dequeued and the structure *values was updated.  If the       */
/*           status QUEUE_IS_EMPTY is returned, the queue was empty and    */
/*           structure *values was not updated.  If the status             */
/*           DATA_WAS_LOST is returned, the queue became full and data has */
/*           been dropped and the structure *values was not updated.       */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI ReadDiscreteQueue(StampedDiscreteType FAR* values);

/***************************************************************************/
/*                                                                         */
/* Function Name: Receive_FIFO                                             */
/*                                                                         */
/* Abstract:  Receives an ARINC word from specified channel.               */
/*                                                                         */
/* Comments: None.                                                         */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI Receive_FIFO(ChannelType Receiver,
                                  ArincWordType FAR* ArincLabel);

/***************************************************************************/
/*                                                                         */
/* Function Name: Receive_FIFO_Block                                       */
/*                                                                         */
/* Abstract: Receives NumOfElem of ARINC words through channel Receiver.   */
/*                                                                         */
/* Comments: This is faster than ReceiveFIFO, if you are receiving a large */
/*           block of arinc words.                                         */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI Receive_FIFO_Block(ChannelType Receiver,
                                        ArincWordType FAR* ArincLabels,
                                        USHORT MaxNumOfLabels,
                                        USHORT FAR* LabelsReceived);

/***************************************************************************/
/*                                                                         */
/* Function Name: Modify_Mailbox_Mode                                     */
/*                                                                         */
/* Abstract: Changes the desired mode for mailbox mode.  This command      */
/*           will cause the mailbox to be reinitialized and possible       */
/*           loss of data will occur.                                      */
/*                                                                         */
/* Comments: None                                                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI Modify_Mailbox_Mode(ChannelType  Channel,
                                         Mailbox_Mode_Type Mailbox_Mode);

/***************************************************************************/
/*                                                                         */
/* Function Name: Receive_Mailbox                                          */
/*                                                                         */
/* Abstract: Get the latest data corresponding to 'Label' for Receiver.    */
/*                                                                         */
/* Comments: None.                                                         */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI Receive_Mailbox(ChannelType Receiver,
									 LabelType Label,
                                     ArincRawWordType FAR* ArincWord);

/***************************************************************************/
/*                                                                         */
/* Function Name: Receive_Mailbox_Extended                                 */
/*                                                                         */
/* Abstract: Get the latest data corresponding to SDI & Label for Receiver.*/
/*                                                                         */
/* Comments: None.                                                         */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI Receive_Mailbox_Extended(ChannelType Receiver,
											  SDI_Type SDI,
											  LabelType Label,
											  ArincRawWordType FAR* ArincWord);

/***************************************************************************/
/*                                                                         */
/* Function Name: SetBitRates                                              */
/*                                                                         */
/* Abstract: Sets the high and low speed data rates of the 3282 chips.     */
/*           Possible parameter values and their respective high speed and */
/*           low speed data rates are shown in the table below.  The high  */
/*           or low speed for a particular frequency pair is set using     */
/*           'SetChipSpeed()'                                              */
/*                                                                         */
/*   Bitrate pair value     High speed data rate   Low speed data rate     */
/*      BITRATE_100_12            100.0KHz               12.5KHz           */
/*      BITRATE_64_8               64.5KHz                8.1KHz           */
/*      BITRATE_80_10              80.0KHz               10.0KHz           */
/*      BITRATE_111_14            111.0KHz               13.9KHz           */
/*                                                                         */
/* Returns 'BAD_ARGUMENT' if card is not initialized or parameter is out   */
/* of range, 'UNSUPPORTED_FEATURE' if the card doesn't support this        */
/* feature or 'PC_FAILURE' if the card is defective.                       */
/*                                                                         */
/* Comments: Upon hardware reset, the data rate pair defaults to           */
/*           100KHz / 12.5KHz.                                             */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI SetBitRates(BitRateType);

/***************************************************************************/
/*                                                                         */
/* Function Name: GetBitRates                                              */
/*                                                                         */
/* Abstract: Gets the card's current bitrate setting.  If the card is      */
/*           uninitialized, 'BAD_ARGUMENT' is returned .  If the card      */
/*           doesn't support this feature, 'UNSUPPORTED_FEATURE' is        */
/*           returned.                                                     */
/*                                                                         */
/* Comments: See above for parameter update return values.                 */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI GetBitRates(BitRateType*);

/***************************************************************************/
/*                                                                         */
/* Function Name: SetChipSpeed                                             */
/*                                                                         */
/* Abstract: Sets a chip for high or low ARINC speed.                      */
/*                                                                         */
/* Comments: None.                                                         */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI SetChipSpeed(ChipType ChipId, SpeedType Speed);

/***************************************************************************/
/*                                                                         */
/* Function Name: GetChipSpeed                                             */
/*                                                                         */
/* Abstract: Gets the chip's current speed setting.                        */
/*                                                                         */
/* Comments: None.                                                         */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI GetChipSpeed(ChipType ChipId, SpeedType FAR* Speed);

/***************************************************************************/
/*                                                                         */
/* Function Name: Shutdown                                                 */
/*                                                                         */
/* Abstract: Stops the 186 on the 429 Card executing.                      */
/*                                                                         */
/* Comments: Must be called for every call made to 'Initialize()' to clean */
/*           up system resources.                                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI Shutdown(VOID);

/***************************************************************************/
/*                                                                         */
/* Function Name: StartCard                                                */
/*                                                                         */
/* Abstract: Starts the 186 on the 429 Card executing.                     */
/*                                                                         */
/* Comments: None.                                                        */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI StartCard(ULONG CardIOAddr, 
                               ResolutionType TimerResolution);

/***************************************************************************/
/*                                                                         */
/* Function Name: Transmit_FIFO                                            */
/*                                                                         */
/* Abstract: Transmits an ARINC word through channel Transmitter.          */
/*                                                                         */
/* Comments: None.                                                         */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI Transmit_FIFO(ChannelType Transmitter,
                                   ArincRawWordType FAR* ArincWord);

/***************************************************************************/
/*                                                                         */
/* Function Name: Transmit_FIFO_Block                                      */
/*                                                                         */
/* Abstract: Transmits NumOfElem of ARINC words through channel.           */
/*                                                                         */
/* Comments: This is faster than Transmit_FIFO, if you are transmitting a  */
/*           large block of arinc words.                                   */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI Transmit_FIFO_Block(ChannelType Transmitter,
                                         ArincRawWordType FAR* ArincWords,
                                         USHORT NumOfElem,
                                         USHORT FAR* NumOfElemTransmitted);

/***************************************************************************/
/*                                                                         */
/* Function Name: Clear_FIFO                                               */
/*                                                                         */
/* Abstract: Clears the FIFO for Receiver.                                 */
/*                                                                         */
/* Comments: None.                                                         */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI Clear_FIFO(ChannelType Receiver);

/***************************************************************************/
/*                                                                         */
/* Function Name: Transmit_Scheduled                                       */
/*                                                                         */
/* Abstract: Transmits an ARINC word through channel Transmitter every     */
/*           "UpdateRate" milliseconds.                                    */
/*                                                                         */
/* Comments: None.                                                         */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI Transmit_Scheduled(ChannelType Transmitter,
                                        USHORT UpdateRate,
                                        ArincRawWordType FAR* ArincLabel);

/***************************************************************************/
/*                                                                         */
/* Function Name: GetScheduledLabelValue                                   */
/*                                                                         */
/* Abstract: Gets the status of a label that has previously been setup for */
/*           Transmition.                                                  */
/*                                                                         */
/* Comments: None.                                                         */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI GetScheduledLabelValue(ChannelType Transmitter,
                                            LabelType Label,
                                            ArincRawWordType FAR* ArincLabel,
                                            USHORT FAR* UpdateRate);

/***************************************************************************/
/*                                                                         */
/* Function Name: WriteDiscrete                                            */
/*                                                                         */
/* Abstract: Writes a value to one of the discrete outputs on the Card.    */
/*                                                                         */
/* Comments: None.                                                         */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI WriteDiscrete(DiscreteType DiscreteNum,
                                   DiscreteValue Value);

/***************************************************************************/
/*                                                                         */
/* Function Name: ComposeArincWord                                         */
/*                                                                         */
/* Abstract: It Composes an Arinc word from its sub parts.                 */
/*                                                                         */
/***************************************************************************/
extern VOID WINAPI ComposeArincWord(ArincRawWordType FAR* AW,
                                    LabelType Label,
									ULONG Data,
									UCHAR SSM);

/***************************************************************************/
/*                                                                         */
/* Function Name: DecomposeArincWord                                       */
/*                                                                         */
/* Abstract: It decompose an Arinc word to its sub parts.                  */
/*                                                                         */
/***************************************************************************/
extern VOID WINAPI DecomposeArincWord(ArincRawWordType FAR* AW,
									  LabelType FAR* Label,
									  ULONG FAR* Data,
									  UCHAR FAR* SSM,
									  ParityType FAR* Parity);

/***************************************************************************/
/*                                                                         */
/* Function Name: TimeDelay                                                */
/*                                                                         */
/* Abstract: Pauses for a specified number of miliseconds.                 */
/*                                                                         */
/* Note: TimeDelay is an approximation, so the delay period will not last  */
/*       exactly Wait miliseconds.                                         */
/*                                                                         */
/* Comments: Use system 'Sleep()' instead.                                 */
/*                                                                         */
/***************************************************************************/
extern VOID WINAPI TimeDelay(LONG Wait);

/***************************************************************************/
/*                                                                         */
/* Function Name: SetDetailedDiagnostic                                    */
/*                                                                         */
/* Abstract: ****** Obsolete ******                                        */
/*           Controls the level of information displayed will running      */
/*           the diagnostic tests.                                         */
/*                                                                         */
/* Comments: Obsolete.  Nothing is performed if called.                    */
/*                                                                         */
/***************************************************************************/
extern VOID WINAPI SetDetailedDiagnostic(PC_BOOLEAN Flag);

/***************************************************************************/
/*                                                                         */
/* Function Name: RunDiagnostic                                            */
/*                                                                         */
/* Abstract: Runs the specified diagnostic test on the board. Returns test */
/*           results of diagnostic, and updates the test result structure  */
/*           described below with more detailed failure information.       */
/*                                                                         */
/* Comments: See 'GetTestResultStruct()'.                                  */
/*                                                                         */
/***************************************************************************/
extern TEST_RESULT WINAPI RunDiagnostic(DiagnosticType Diagnostic,
                                        CHAR FAR* Unused,
										CHAR FAR* HexFile,
                                        CardType Card);

/***************************************************************************/
/*                                                                         */
/* Function Name: GetTestResultStruct()                                    */
/*                                                                         */
/* Abstract: Returns the address of the structure used for low level       */
/*           diagnostics.  'Initialize()' and 'RunDiagnostic()' called     */
/*           with the MEMORY_CHECK or ROM_FIFO_CHECK option update this    */
/*           structure upon the first failure in the test.                 */
/*                                                                         */
/* Comments: None.                                                         */
/*                                                                         */
/***************************************************************************/
extern TestResultStructType* WINAPI GetTestResultStructAddr(VOID);

/***************************************************************************/
/*                                                                         */
/* Function Name: ArincToTransceiver                                       */
/*                                                                         */
/* Abstract: Runs the specified diagnostic test on board.                  */
/*                                                                         */
/* Comments: Converts from arinc format to transceiver arinc.  This        */
/*           function is applied to the data being transmitted.            */
/*                                                                         */
/***************************************************************************/
extern ULONG WINAPI ArincToTransceiver(ULONG Data);

/***************************************************************************/
/*                                                                         */
/* Function Name: TransceiverToArinc                                       */
/*                                                                         */
/* Abstract: Runs the specified diagnostic test on board.                  */
/*                                                                         */
/* Comments: Converts from transceiver format to true arinc.  This is      */
/*           applied to the data being received.                           */
/*                                                                         */
/***************************************************************************/
extern ULONG WINAPI TransceiverToArinc(ULONG Data);

/***************************************************************************/
/*                                                                         */
/* Function Name: Get429TimerResolution                                    */
/*                                                                         */
/* Abstract: Returns the current Timer resolution.                         */
/*                                                                         */
/* Comments: None.                                                         */
/*                                                                         */
/***************************************************************************/
extern ResolutionType WINAPI Get429TimerResolution(VOID);

/***************************************************************************/
/*                                                                         */
/* Function Name: GetCurrentCard                                           */
/*                                                                         */
/* Abstract: Gets the current active card.                                 */
/*                                                                         */
/* Comments: Cards are numbered from 1 to N.                               */
/*                                                                         */
/***************************************************************************/
extern UCHAR WINAPI GetCurrentCard(VOID);

/***************************************************************************/
/*                                                                         */
/* Function Name: SetCurrentCard                                           */
/*                                                                         */
/* Abstract: Sets the active card.                                         */
/*                                                                         */
/* Comments: Cards are numbered from 1 to N.                               */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI SetCurrentCard(UCHAR CardNum);

/***************************************************************************/
/*                                                                         */
/* Function Name: GetCardCurrentTime                                       */
/*                                                                         */
/* Abstract: Get the 186's timer2 (card's timer) time                      */
/*                                                                         */
/* Comments:                                                               */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI GetCardCurrentTime(USHORT FAR* SoftwareTime,
                                        ULONG FAR* HardwareTime);

/***************************************************************************/
/*                                                                         */
/* Function Name: ZeroCardClock                                            */
/*                                                                         */
/* Abstract: Reset the 186's timer2 (card's timer) time.                   */
/*                                                                         */
/* Comments: This function suspends interrupts to the 186, so there may    */
/*           be a hiccup (discontinuity) in scheduled transmissions and    */
/*           frequency driven interrupts to the PC.  Additionally data     */
/*           coming in on the card's 429 receivers may be lost (since the  */
/*           receiver interrupts to the 186 won't be enabled).             */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI ZeroCardClock(VOID);

#ifndef _NI_i386_ /* not being compiled by Labwindows/CVI */

/***************************************************************************/
/*                                                                         */
/* Function Name: Get429TimeDifference                                     */
/*                                                                         */
/* Abstract:                                                               */
/*                                                                         */
/* Comments:                                                               */
/*                                                                         */
/***************************************************************************/
extern float WINAPI Get429TimeDifference(ArincWordType FAR* PrevAW,
                                         ArincWordType FAR* CurAW);

/***************************************************************************/
/*                                                                         */
/* Function Name: Get429AbsoluteTime                                       */
/*                                                                         */
/* Abstract:                                                               */
/*                                                                         */
/* Comments:                                                               */
/*                                                                         */
/***************************************************************************/
extern float WINAPI Get429AbsoluteTime(register ArincWordType FAR* AW);
#endif /* Labwindows/CVI */

/***************************************************************************/
/*                                                                         */
/* Function Name: SetInitialAbsoluteTime                                   */
/*                                                                         */
/* Abstract:                                                               */
/*                                                                         */
/* Comments:                                                               */
/*                                                                         */
/***************************************************************************/
extern VOID WINAPI SetInitialAbsoluteTime(USHORT TimeInMilliSeconds);

/***************************************************************************/
/*                                                                         */
/* Function Name: SetChannelSpeed                                          */
/*                                                                         */
/* Abstract:                                                               */
/*                                                                         */
/* Comments:                                                               */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI SetChannelSpeed(ChannelType Channel, SpeedType Speed);

/***************************************************************************/
/*                                                                         */
/* Function Name: SetChannelParity                                         */
/*                                                                         */
/* Abstract:                                                               */
/*                                                                         */
/* Comments:                                                               */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI SetChannelParity(ChannelType Channel, 
                                      ParityType Parity);

/***************************************************************************/
/*                                                                         */
/* Function Name: EnableParityAsData                                       */
/*                                                                         */
/* Abstract:                                                               */
/*                                                                         */
/* Comments:                                                               */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI EnableParityAsData(ChannelType Channel,
                                        PC_BOOLEAN Switch);

/***************************************************************************/
/*                                                                         */
/* Function Name: GetChannelSpeed                                          */
/*                                                                         */
/* Abstract:                                                               */
/*                                                                         */
/* Comments:                                                               */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI GetChannelSpeed(ChannelType Channel,
                                     SpeedType FAR* Speed);

/***************************************************************************/
/*                                                                         */
/* Function Name: ToolBoxReadConfigFile                                    */
/*                                                                         */
/* Abstract: ****** Obsosete ******                                        */
/*                                                                         */
/* Comments: Low level backward compatible call.  Not to be used!          */
/*                                                                         */
/***************************************************************************/
#ifndef _NI_I386_
extern STATUS FAR CDECL _export ToolBoxReadConfigFile(LPSTR fname,
                                                  SHORTINT NumberOfArg, ...);
#endif

/***************************************************************************/
/*                                                                         */
/* Function Name: ToolBoxReadConfigInfo                                    */
/*                                                                         */
/* Abstract: Returns the configuration information for the current         */
/*           card selected.                                                */
/*                                                                         */
/* Comments: Low level backward compatible call.  Not to be used!          */
/*                                                                         */
/***************************************************************************/
extern STATUS FAR CDECL _export ToolBoxReadConfigInfo(LPLONG progAddr,
                                                      LPLONG dpramAddr,
                                                      LPLONG ioAddr,
                                                      LPINT Unused);
    
/***************************************************************************/
/*                                                                         */
/* Function Name: EnableGlobalCardToPcInt                                  */
/*                                                                         */
/* Abstract: Causes card to generate periodic interrupts or interrupts     */
/*           upondata reception, and defines the routine to run in         */
/*           response to such events.                                      */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI EnableGlobalCardToPcInt(SHORTINT Interval,
                                             SHORTINT Unused1,
                                             SHORTINT Unused2,
                                             FARPROC PC_ReceiverIntRoutine);

/***************************************************************************/
/*                                                                         */
/* Function Name: DisableGlobalCardToPcInt                                 */
/*                                                                         */
/* Abstract: Halts periodic interrupts from card.                          */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI DisableGlobalCardToPcInt(VOID);

/***************************************************************************/
/*                                                                         */
/* Function Name: ClearPCInterrupt                                         */
/*                                                                         */
/* Abstract: ****** Obsolete ******                                        */
/*                allows the PC to clear an "interrupt" sent from the      */
/*                429 card.  It does this by reading from port 3x4 (where  */
/*                x depends on the card's IO address).  It also clears     */
/*                the interrupt flag in DPRAM which is set by the board    */
/*                code when raising a PC interrupt.                        */
/*                                                                         */
/* Comments: Obsolete.  Nothing is performed if called.                    */
/*                                                                         */
/***************************************************************************/
extern STATUS WINAPI ClearPCInterrupt(SHORT CardNumber);

/***************************************************************************/
/*                                                                         */
/* Function Name: P429T_version, P429T_VerStr                              */
/*                                                                         */
/* Abstract: returns a pointer to a string containing the version.         */
/*                                                                         */
/***************************************************************************/
extern LPSTR WINAPI P429TB_version(VOID);
/* example: "1.4.2" */

extern LPSTR WINAPI P429T_VerStr(VOID);
/* example: "PCI429.DLL - Ver 1.4.2" */

/***************************************************************************/
/*                                                                         */
/* Function Name: P429TBVersion                                            */
/*                                                                         */
/* Abstract: Places the a version string into the string array passed      */ 
/*           to it.  (Make sure that the string array passed has at        */ 
/*           least enough room for 25 characters).                         */
/*                                                                         */
/***************************************************************************/
extern BOOL  WINAPI P429TBVersion(LPSTR lpVerNbr);
/* example: "PCI429.DLL - Ver 1.4.2" */

/***************************************************************************/
/*                                                                         */
/* Function Name: Create429MsgWindow                                       */
/*                                                                         */
/* Abstract: ****** Obsolete ******                                        */
/*           Creates a message window at position.                         */
/*           Note:if nWidth or nHeight equal = 0 Default Windows size.     */
/*                                                                         */
/* Comments: Obsolete.  Nothing is performed if called.                    */
/*                                                                         */
/***************************************************************************/
extern BOOL  WINAPI Create429MsgWindow(SHORTINT nHorizontal,
                                       SHORTINT nVertical,
                                       SHORTINT nWidth,
                                       SHORTINT nHeight);

/***************************************************************************/
/*                                                                         */
/* Function Name: Close429MsgWindow                                        */
/*                                                                         */
/* Abstract: ****** Obsolete ******                                        */
/*           Close a message window.                                       */
/*                                                                         */
/* Comments: Obsolete.  Nothing is performed if called.                    */
/*                                                                         */
/***************************************************************************/
extern VOID  WINAPI Close429MsgWindow(VOID);

/***************************************************************************/
/*                                                                         */
/* Function Name: GetChannelQueue                                          */
/*                                                                         */
/* Abstract: Returns the reference to message queues.                      */
/*                                                                         */
/* Comments: For internal use only.                                        */
/*                                                                         */
/***************************************************************************/
extern PC_BOOLEAN WINAPI GetChannelQueue( ChannelType Channel,		
                                          QueueInformation * pQueueInfo );

#ifdef __cplusplus
}
#endif
/**************************************************************************/
#endif   /* #ifndef PROTO429    */
/*************************** End of file ***********************************/
 