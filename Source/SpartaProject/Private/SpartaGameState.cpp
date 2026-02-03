// Fill out your copyright notice in the Description page of Project Settings.

#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 30.f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
	CurrentWaveIndex = 0;
	MaxWavesPerLevel = 3;

	// 기본 웨이브 정보 설정 (3개 레벨 x 3개 웨이브 = 9개)
	// Level 1 - BasicLevel
	WaveInfos.Add(FWaveInfo(20, 30.f));  // Wave 1: 20개 아이템, 30초
	WaveInfos.Add(FWaveInfo(30, 25.f));  // Wave 2: 30개 아이템, 25초
	WaveInfos.Add(FWaveInfo(40, 20.f));  // Wave 3: 40개 아이템, 20초

	// Level 2 - IntermediateLevel
	WaveInfos.Add(FWaveInfo(30, 25.f));  // Wave 1: 30개 아이템, 25초
	WaveInfos.Add(FWaveInfo(40, 20.f));  // Wave 2: 40개 아이템, 20초
	WaveInfos.Add(FWaveInfo(50, 18.f));  // Wave 3: 50개 아이템, 18초

	// Level 3 - AdvancedLevel
	WaveInfos.Add(FWaveInfo(40, 20.f));  // Wave 1: 40개 아이템, 20초
	WaveInfos.Add(FWaveInfo(50, 18.f));  // Wave 2: 50개 아이템, 18초
	WaveInfos.Add(FWaveInfo(60, 15.f));  // Wave 3: 60개 아이템, 15초
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASpartaGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	// 현재 레벨의 첫 번째 웨이브부터 시작
	CurrentWaveIndex = 0;
	StartWave();
}

void ASpartaGameState::StartWave()
{
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	// 현재 레벨과 웨이브에 맞는 인덱스 계산
	int32 WaveInfoIndex = (CurrentLevelIndex * MaxWavesPerLevel) + CurrentWaveIndex;

	if (!WaveInfos.IsValidIndex(WaveInfoIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("[GameState] Invalid WaveInfo index: %d"), WaveInfoIndex);
		return;
	}

	FWaveInfo CurrentWave = WaveInfos[WaveInfoIndex];

	// 화면에 웨이브 시작 알림 표시
	FString WaveMessage = FString::Printf(TEXT("Level %d - Wave %d 시작!"), 
		CurrentLevelIndex + 1, CurrentWaveIndex + 1);
	
	GEngine->AddOnScreenDebugMessage(
		-1, 
		3.f, 
		FColor::Yellow, 
		WaveMessage, 
		true, 
		FVector2D(2.f, 2.f)
	);

	UE_LOG(LogTemp, Warning, TEXT("[GameState] %s - Items: %d, Duration: %.1f"), 
		*WaveMessage, CurrentWave.ItemCount, CurrentWave.Duration);

	// SpawnVolume 찾기 및 DataTable 설정
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	if (FoundVolumes.Num() > 0)
	{
		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
		if (SpawnVolume)
		{
			// 현재 레벨/웨이브에 맞는 DataTable 설정
			SpawnVolume->SetCurrentDataTableIndex(CurrentLevelIndex, CurrentWaveIndex);

			// 아이템 스폰
			for (int32 i = 0; i < CurrentWave.ItemCount; i++)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				// 만약 스폰된 액터가 코인 타입이라면 SpawnedCoinCount 증가
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	// 웨이브 타이머 설정
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ASpartaGameState::OnWaveTimeUp,
		CurrentWave.Duration,
		false);

	UpdateHUD();

	UE_LOG(LogTemp, Warning, TEXT("Level %d - Wave %d Start! Spawned %d coins"),
		CurrentLevelIndex + 1, CurrentWaveIndex + 1, SpawnedCoinCount);
}

void ASpartaGameState::OnLevelTimeUp()
{
	// 기존 함수 호환성 유지
	OnWaveTimeUp();
}

void ASpartaGameState::OnWaveTimeUp()
{
	UE_LOG(LogTemp, Warning, TEXT("[GameState] Wave %d time's up!"), CurrentWaveIndex + 1);
	CheckWaveCompletion();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	UE_LOG(LogTemp, Warning, TEXT("Coin Collected! Total: %d / %d"),
		CollectedCoinCount, SpawnedCoinCount);

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameState] All coins collected in Wave %d!"), CurrentWaveIndex + 1);
		CheckWaveCompletion();
	}
}

void ASpartaGameState::CheckWaveCompletion()
{
	// 타이머 해제
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	CurrentWaveIndex++;

	// 현재 레벨의 모든 웨이브를 완료했는지 확인
	if (CurrentWaveIndex >= MaxWavesPerLevel)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameState] Level %d completed! All waves finished."), CurrentLevelIndex + 1);
		
		// 화면에 레벨 완료 메시지 표시
		GEngine->AddOnScreenDebugMessage(
			-1, 
			3.f, 
			FColor::Green, 
			FString::Printf(TEXT("Level %d 완료!"), CurrentLevelIndex + 1), 
			true, 
			FVector2D(2.f, 2.f)
		);

		EndLevel();
	}
	else
	{
		// 다음 웨이브 시작
		UE_LOG(LogTemp, Warning, TEXT("[GameState] Wave %d completed! Starting next wave..."), CurrentWaveIndex);
		
		// 화면에 웨이브 완료 메시지 표시
		GEngine->AddOnScreenDebugMessage(
			-1, 
			2.f, 
			FColor::Cyan, 
			FString::Printf(TEXT("Wave %d 완료! 다음 웨이브 준비..."), CurrentWaveIndex), 
			true, 
			FVector2D(2.f, 2.f)
		);

		// 짧은 딜레이 후 다음 웨이브 시작
		FTimerHandle NextWaveTimerHandle;
		GetWorldTimerManager().SetTimer(
			NextWaveTimerHandle,
			this,
			&ASpartaGameState::StartWave,
			2.0f,
			false);
	}
}

void ASpartaGameState::EndLevel()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			// 다음 레벨로 이동
			CurrentLevelIndex++;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;

			UE_LOG(LogTemp, Warning, TEXT("[GameState] EndLevel - Moving to level index: %d"), CurrentLevelIndex);

			// 모든 레벨을 다 돌았다면 게임 오버
			if (CurrentLevelIndex >= MaxLevels)
			{
				UE_LOG(LogTemp, Warning, TEXT("[GameState] All levels completed! Game Over!"));
				OnGameOver();
				return;
			}

			// 레벨 맵 이름이 있다면 해당 맵 불러오기
			if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
			{
				FName NextLevelName = LevelMapNames[CurrentLevelIndex];
				UE_LOG(LogTemp, Warning, TEXT("[GameState] Opening next level: %s"), *NextLevelName.ToString());
				UGameplayStatics::OpenLevel(GetWorld(), NextLevelName);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[GameState] No level name found for index %d! Going to Game Over."), CurrentLevelIndex);
				OnGameOver();
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[GameState] Failed to cast to SpartaGameInstance!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[GameState] GameInstance is NULL!"));
	}
}

void ASpartaGameState::OnGameOver()
{
	UE_LOG(LogTemp, Warning, TEXT("[GameState] Game Over called"));
	
	// 화면에 게임 오버 메시지 표시
	GEngine->AddOnScreenDebugMessage(
		-1, 
		5.f, 
		FColor::Red, 
		TEXT("모든 레벨 완료! Game Over!"), 
		true, 
		FVector2D(2.5f, 2.5f)
	);

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d - Wave: %d"), 
						CurrentLevelIndex + 1, CurrentWaveIndex + 1)));
				}
			}
		}
	}
}