// SMA01(29Dec2021): Why does the run overflow for an input of
// more than 2147483647? This means that the variable in neither
// 64 bit, nor unsigned

// SMA01 (29Dec2021): This file is modified to introduce
// more complicated features such as dynamic memory
// assignment, print, and scan, features

/** @file
  This sample application bases on HelloWorld PCD setting
  to print "UEFI Hello World!" to the UEFI Console.

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>

// Added to define "gBS" in the defined "malloc" and "free" functions
#include <Library/UefiBootServicesTableLib.h>

//
// String token ID of help message text.
// Shell supports to find help message in the resource section of an application image if
// .MAN file is not found. This global variable is added to make build tool recognizes
// that the help string is consumed by user and then build tool will add the string into
// the resource section. Thus the application can use '-?' option to show help message in
// Shell.
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_STRING_ID  mStringHelpTokenId = STRING_TOKEN(STR_HELLO_WORLD_HELP_INFORMATION);

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/

#define My_UINT_Ex UINT64

struct My128BitSignedInt
{
	// The number is assumed to be of form (X.Y)_2
	// For example 1567.6579 which is
	//  00000000
	//  0000061F.
	//  00000000
	//  000019B3 would mean
	//  6*16^2+1*16+15+1/(16^13)+9/(16^14)+11/(16^15)+3/(16^16)
	//  from HEX or
	//  Y.00000000
	//  00000000
	//  00000000
	//  00000000
	//  00000000
	//  00000000
	//  00011001
	//  10110011
	//  from binary which are both
	//  1567.0000000000000003566483046391066835667515988461673259735107421875
	My_UINT_Ex AfterDecimal;
	My_UINT_Ex BeforeDecimal;
	BOOLEAN Sign;
};

My_UINT_Ex My_Fetch_An_Int64(IN EFI_SYSTEM_TABLE*);
void* malloc(UINTN);
void free(void*);

EFI_INPUT_KEY Pressed_Key;

EFI_STATUS
EFIAPI
UefiMain(
	IN EFI_HANDLE        ImageHandle,
	IN EFI_SYSTEM_TABLE* SystemTable
)
{
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"In the name of Allah\r\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"This program is developed by SMA01\r\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"for checking the effective cpu speed\r\n");

	My_UINT_Ex MaxLoopCounter = 1000;
	My_UINT_Ex LoopCounter = 0;
	My_UINT_Ex Inputed_Number = 0;
	My_UINT_Ex Tally = 0;
	My_UINT_Ex TallyIncreament = 0x0000000000000001;
	while (LoopCounter < MaxLoopCounter)
	{
		Inputed_Number = My_Fetch_An_Int64(SystemTable);
		Print(L"%d\r\n", Inputed_Number);

		Tally = 0;
		for (My_UINT_Ex Counter = 0; Counter < Inputed_Number; Counter++)
		{
			Tally += TallyIncreament;
		}
		Print(L"The sum is equal to %d\r\n", Tally);
		LoopCounter++;
	}

	return EFI_SUCCESS;
}

My128BitSignedInt My128BitSummer(My128BitSignedInt a, My128BitSignedInt b)
{
	My128BitSignedInt Result;

	if (a.Sign == FALSE && b.Sign == FALSE)
	{
		// This is the case of both positive numbers
		Result.AfterDecimal = a.AfterDecimal + b.AfterDecimal;
		if (Result.AfterDecimal < a.AfterDecimal || Result.AfterDecimal < b.AfterDecimal) {
			Result.BeforeDecimal = 1 + a.BeforeDecimal + b.BeforeDecimal;
		}
		else {
			Result.BeforeDecimal = a.BeforeDecimal + b.BeforeDecimal;
		}
	}

	return result;
}

My_UINT_Ex My_Fetch_An_Int64(IN EFI_SYSTEM_TABLE* SystemTable)
{
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Enter an integer number, then press the return key\r\n");
	BOOLEAN Finished_Reading = FALSE;
	UINTN Key_Event = 0;
	My_UINT_Ex MaxChars = 1000;
	CHAR16* Entered_String = malloc(MaxChars * sizeof(CHAR16));
	My_UINT_Ex Current_Index = 0;
	while (Finished_Reading == FALSE)
	{
		SystemTable->BootServices->WaitForEvent(1, &(SystemTable->ConIn->WaitForKey), &Key_Event);
		SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Pressed_Key);
		SystemTable->ConIn->Reset(SystemTable->ConIn, FALSE);
		if (Pressed_Key.UnicodeChar != 13)
		{
			if (Pressed_Key.UnicodeChar >= 48 &&
				Pressed_Key.UnicodeChar <= 57)
			{
				if (Current_Index < MaxChars)
				{
					Entered_String[Current_Index] = Pressed_Key.UnicodeChar;
				}
				else
				{
					// Resizing should be placed here in an advanced code
					// Here, we would do nothing, except for a warning
					Print(L"Number of entered characters is bigger than the assigned buffer\r\n");
				}
				Current_Index++;
			}
			else if (Pressed_Key.UnicodeChar == 8)
			{
				if (Current_Index != 0)
				{
					Current_Index = Current_Index - 1;
				}
				else
				{
					Current_Index = 0;
				}
			}
			Print(L"%c", Pressed_Key.UnicodeChar);
		}
		else
		{
			Print(L"\r\n");
			Finished_Reading = TRUE;
		}
	}
	My_UINT_Ex Final_Result = 0;
	for (int Digit_Counter = 0; Digit_Counter < Current_Index; Digit_Counter++)
	{
		Final_Result = Final_Result * 10 + (INT64)(Entered_String[Digit_Counter] - 48);
	}

	free(Entered_String);
	// The printing format specifier is detailed in:
	// https://github.com/tianocore/edk2/blob/master/MdePkg/Include/Library/PrintLib.h
	Print(L"Your input number is equal to %d\r\n", Final_Result);
	return Final_Result;
}

void* malloc(UINTN poolSize)
{
	EFI_STATUS status;
	void* handle;
	// Print(L"allocating memory pool\n");

	// SMA01: Since we are compiling in WINDOWS, this should be changed
	// status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiLoaderData, poolSize, &handle);
	status = gBS->AllocatePool(EfiLoaderData, poolSize, &handle);

	if (status == EFI_OUT_OF_RESOURCES)
	{
		Print(L"out of resources for pool\n");
		return 0;
	}
	else if (status == EFI_INVALID_PARAMETER)
	{
		Print(L"invalid pool type\n");
		return 0;
	}
	else
	{
		// Print(L"memory pool successfully allocated\n");
		// Do nothing
		return handle;
	}
}

void free(void* pool)
{
	EFI_STATUS status;
	// Print(L"freeing memory pool\n");

	// SMA01: Since we are compiling in WINDOWS, this should be changed
	// status = uefi_call_wrapper(gBS->FreePool, 1, pool);
	status = gBS->FreePool(pool);

	if (status == EFI_INVALID_PARAMETER)
	{
		Print(L"invalid pool pointer\n");
	}
	else
	{
		// Print(L"memory pool successfully freed\n");
		// Do nothing
	}
}