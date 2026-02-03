// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

// 웨이브 정보 구조체
USTRUCT(BlueprintType)
struct FWaveInfo
{
	GENERATED_BODY()

	// 웨이브에서 스폰할 아이템 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemCount;

	// 웨이브 제한 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;

	FWaveInfo()
		: ItemCount(30)
		, Duration(30.f)
	{
	}

	FWaveInfo(int32 InItemCount, float InDuration)
		: ItemCount(InItemCount)
		, Duration(InDuration)
	{
	}
};

UCLASS()
class SPARTAPROJECT_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()

public:
	ASpartaGameState();
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	// 현재 레벨에서 스폰된 코인 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	// 플레이어가 수집한 코인 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;
	// 각 레벨이 유지되는 시간
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float LevelDuration;
	// 현재 진행 중인 레벨 인덱스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	// 총 레벨 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	// 레벨 맵 이름 배열
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	// 웨이브 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWavesPerLevel;

	// 각 레벨의 웨이브 정보 (블루프린트에서 편집 가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TArray<FWaveInfo> WaveInfos;

	// 매 레벨이 끝나기 전까지 시간이 흐르도록 관리하는 타이머
	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	// 게임이 완전히 끝났을 때 모든 레벨 종료
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();

	// 레벨을 시작할 때, 아이템 스폰 및 타이머 설정
	void StartLevel();
	// 웨이브 시작
	void StartWave();
	// 웨이브 종료 체크
	void CheckWaveCompletion();
	// 레벨 제한 시간이 만료되었을 때 호출
	void OnWaveTimeUp();
	// 레벨 제한 시간이 만료되었을 때 호출 (기존 함수 - 호환성)
	void OnLevelTimeUp();
	// 코인을 주웠을 때 호출
	void OnCoinCollected();
	// 레벨을 강제 종료, 다음 레벨로 이동
	void EndLevel();
	void UpdateHUD();
};
