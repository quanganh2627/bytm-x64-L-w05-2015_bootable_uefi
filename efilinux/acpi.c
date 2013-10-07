/*
 * Copyright (c) 2013, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other materials provided with the
 *      distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <efi.h>
#include <efilib.h>
#include "acpi.h"
#include "utils.h"

EFI_STATUS get_rsdt_table(struct RSDT_TABLE **rsdt)
{
	EFI_GUID acpi2_guid = ACPI_20_TABLE_GUID;
	struct RSDP_TABLE *rsdp;
	EFI_STATUS ret;

	ret = LibGetSystemConfigurationTable(&acpi2_guid, (VOID **)&rsdp);
	if (EFI_ERROR(ret)) {
		error(L"Failed to retrieve ACPI 2.0 table", ret);
		goto out;
	}

	if (strncmpa((CHAR8 *)rsdp->signature, (CHAR8 *)RSDP_SIG, sizeof(RSDP_SIG) - 1)) {
		CHAR8 *s = rsdp->signature;
		Print(L"RSDP table has wrong signature (%c%c%c%c%c%c%c%c)\n",
		      s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7]);
		ret = EFI_COMPROMISED_DATA;
		goto out;
	}

	*rsdt = (struct RSDT_TABLE *)rsdp->rsdt_address;
	if (strncmpa((CHAR8 *)(*rsdt)->header.signature, (CHAR8 *)RSDT_SIG, sizeof(RSDT_SIG) - 1)) {
		CHAR8 *s = (*rsdt)->header.signature;
		Print(L"RSDT table has wrong signature (%c%c%c%c)\n", s[0], s[1], s[2], s[3]);
		ret = EFI_COMPROMISED_DATA;
		goto out;
	}
out:
	return ret;
}

EFI_STATUS list_acpi_tables(void)
{
	struct RSDT_TABLE *rsdt;
	EFI_STATUS ret;

	ret = get_rsdt_table(&rsdt);
	if (EFI_ERROR(ret))
		return ret;

	int nb_acpi_tables = (rsdt->header.length - sizeof(rsdt->header)) / sizeof(rsdt->entry[1]);
	Print(L"Listing %d tables\n", nb_acpi_tables);

	int i;
	for (i = 0 ; i < nb_acpi_tables; i++) {
		CHAR8 *s = ((struct ACPI_DESC_HEADER *)rsdt->entry[i])->signature;
		Print(L"RSDT[%d] = %c%c%c%c\n", i, s[0], s[1], s[2], s[3]);
	}

	return EFI_SUCCESS;
}

EFI_STATUS get_acpi_table(CHAR8 *signature, VOID **table)
{
	struct RSDT_TABLE *rsdt;
	EFI_STATUS ret;
	int nb_acpi_tables;
	int i;

	ret = get_rsdt_table(&rsdt);
	if (EFI_ERROR(ret))
		goto out;

	nb_acpi_tables = (rsdt->header.length - sizeof(rsdt->header)) / sizeof(rsdt->entry[1]);
	ret = EFI_NOT_FOUND;
	for (i = 0 ; i < nb_acpi_tables; i++) {
		struct ACPI_DESC_HEADER *header;
		header = (struct ACPI_DESC_HEADER *)rsdt->entry[i];
		if (!strncmpa(header->signature, signature, strlena(signature))) {
			debug(L"Found %c%c%c%c table\n", signature[0], signature[1], signature[2], signature[3]);
			*table = (struct PIDV_TABLE *)header;
			ret = EFI_SUCCESS;
			break;
		}
	}
out:
	return ret;
}


enum flow_types acpi_read_flow_type(void)
{
        /* TODO */
	debug(L"TO BE IMPLEMENTED\n");
	return FLOW_NORMAL;
}

void acpi_cold_off(void)
{
        /* TODO */
	debug(L"TO BE IMPLEMENTED\n");
	return;
}

EFI_STATUS rsci_populate_indicators(void)
{
        /* TODO */
	debug(L"TO BE IMPLEMENTED\n");
	return EFI_SUCCESS;
}

void print_pidv(struct PIDV_TABLE *pidv)
{
	int i;
	Print(L"part_number      	=");
	for (i				= 0; i < sizeof(pidv->part_number); i++)
		Print(L"%x", pidv->part_number[i]);
	Print(L"\n");
	Print(L"ext_id_1 x_id1:\n");
	Print(L"customer		=0x%x\n",pidv->x_id1.customer);
	Print(L"vendor			=0x%x\n",pidv->x_id1.vendor);
	Print(L"device_manufacturer	=0x%x\n",pidv->x_id1.device_manufacturer);
	Print(L"platform_family		=0x%x\n",pidv->x_id1.platform_family);
	Print(L"product_line		=0x%x\n",pidv->x_id1.product_line);
	Print(L"hardware		=0x%x\n",pidv->x_id1.hardware);
	Print(L"spid_checksum		=0x%x\n",pidv->x_id1.spid_checksum);
	Print(L"fru			=0x%x\n",pidv->x_id1.fru);
	Print(L"fru_checksum		=0x%x\n",pidv->x_id1.fru_checksum);
	Print(L"reserved		=0x%x\n",pidv->x_id1.reserved);
	Print(L"data1			=0x%x\n", pidv->x_id2.data1);
	Print(L"data2			=0x%x\n", pidv->x_id2.data2);
	Print(L"data3			=0x%x\n", pidv->x_id2.data3);
	Print(L"data4			=0x%x\n", pidv->x_id2.data4);
	Print(L"ext_id_2 x_id2:\n");
	Print(L"system_uuid		=0x%x\n", pidv->system_uuid);
}

void print_rsci(struct RSCI_TABLE *rsci)
{
	Print(L"wake_source	=0x%x\n", rsci->wake_source);
	Print(L"reset_source	=0x%x\n", rsci->reset_source);
	Print(L"reset_type	=0x%x\n", rsci->reset_type);
	Print(L"shutdown_source	=0x%x\n", rsci->shutdown_source);
	Print(L"indicators	=0x%x\n", rsci->indicators);
}