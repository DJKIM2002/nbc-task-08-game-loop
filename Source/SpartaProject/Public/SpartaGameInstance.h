// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SpartaGameInstance.generated.h"

UCLASS()
class SPARTAPROJECT_API USpartaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USpartaGameInstance();

	// 게임 전체 누적 점수
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 TotalScore;
	// 현재 레벨 인덱스
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 CurrentLevelIndex;

	UFUNCTION(BlueprintCallable, Category = "GameData")
	void AddToScore(int32 Amount);
};
