#define _CRT_SECURE_NO_WARNINGS


#include "SocketSendRecvTools.h"
#include <stdio.h>
#include <string.h>


TransferResult_t SendBuffer(const char* Buffer, int BytesToSend, SOCKET sd)
{
	const char* CurPlacePtr = Buffer;
	int BytesTransferred;
	int RemainingBytesToSend = BytesToSend;

	while (RemainingBytesToSend > 0)
	{
		/* send does not guarantee that the entire message is sent */
		BytesTransferred = send(sd, CurPlacePtr, RemainingBytesToSend, 0);
		if (BytesTransferred == SOCKET_ERROR)
		{
			printf("send() failed, error %d\n", WSAGetLastError());
			return TRNS_FAILED;
		}

		RemainingBytesToSend -= BytesTransferred;
		CurPlacePtr += BytesTransferred; // <ISP> pointer arithmetic
	}

	return TRNS_SUCCEEDED;
}



TransferResult_t SendString(char *Str, SOCKET sd)
{

	/*****************************************************************************************************************/
	/*Parameters: Str - the string to send. sd - the socket used for communication.								     */
	/*Return:  TRNS_SUCCEEDED - if sending succeeded. TRNS_FAILED - otherwise										 */
	/*Description: uses a socket to send a string.							      								     */
	/*****************************************************************************************************************/

	/* Send the the request to the server on socket sd */
	int TotalStringSizeInBytes;
	TransferResult_t SendRes;

	TotalStringSizeInBytes = (int)(strlen(Str) + 1); // terminating zero also sent	
	char *Buffer = (char*)malloc(TotalStringSizeInBytes * sizeof(char));
	strcpy(Buffer, Str);
	Buffer[strlen(Str)] = '\0';
	SendRes = SendBuffer((const char *)(Buffer), (int)(TotalStringSizeInBytes), sd);
	if (SendRes != TRNS_SUCCEEDED) return SendRes;

	return SendRes;
}


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

TransferResult_t ReceiveString(char** OutputStrPtr, SOCKET sd) {
	/*****************************************************************************************************************/
	/*Parameters: OutputStrPtr - a pointer to a char-pointer that is initialized to NULL. d - the socket used for    */
	/*communication.																							     */
	/*Return:  TRNS_SUCCEEDED - if receiving and memory allocation succeeded,										 */
	/*TRNS_DISCONNECTED - if the socket was disconnected .TRNS_FAILED - otherwise									 */
	/*Description: uses a socket to receive a string, and stores it in dynamic memory.								 */
	/*****************************************************************************************************************/

	TransferResult_t RecvRes = TRNS_SUCCEEDED;
	char* StrBuffer = NULL;

	if ((OutputStrPtr == NULL) || (*OutputStrPtr != NULL)) {
		return TRNS_FAILED;
	}
	StrBuffer = (char*)malloc(100 * sizeof(char));

	if (StrBuffer == NULL) {
		return TRNS_FAILED;
	}

	char* CurPlacePtr = StrBuffer;
	int BytesJustTransferred;

	while (1) {
		/* send does not guarantee that the entire message is sent */
		BytesJustTransferred = recv(sd, CurPlacePtr, 100 - (CurPlacePtr - StrBuffer), 0);
		if (BytesJustTransferred == SOCKET_ERROR) {
			return TRNS_FAILED;
		}
		else if (BytesJustTransferred == 0) {
			return TRNS_DISCONNECTED;
		} // recv() returns zero if connection was gracefully disconnected.

		CurPlacePtr += BytesJustTransferred; // <ISP> pointer s
		
		if (*(CurPlacePtr - 1) == '\0') {
			break;
		}
	}


	if (RecvRes == TRNS_SUCCEEDED) {
		*OutputStrPtr = StrBuffer;
	}
	else {
		free(StrBuffer);
	}

	return RecvRes;
}
