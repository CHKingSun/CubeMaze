// Fill out your copyright notice in the Description page of Project Settings.


#include "EntryPortalActor.h"


int32 AEntryPortalActor::GetBindPortalEntry()const
{
	if (const auto Entry = Cast<AEntryPortalActor>(PortalBind))
	{
		return Entry->EntryPos;
	}

	return -1;
}
