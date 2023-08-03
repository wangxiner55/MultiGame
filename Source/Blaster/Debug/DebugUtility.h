#pragma once


namespace DebugUtility
{


	static void PrintPass(const FString& Message)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, Message);
	}

	static void PrintWarn(const FString& Message)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, Message);
	}

	static void PrintError(const FString& Message)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, Message);
	}

	static void Print(const FString& Message, const FColor& Color)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0, Color, Message);
	}
}