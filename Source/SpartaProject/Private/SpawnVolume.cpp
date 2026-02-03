// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	// 박스 컴포넌트를 생성하고, 이 액터의 루트로 설정
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);
}

AActor* ASpawnVolume::SpawnRandomItem()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}
	return nullptr;
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	// 박스 컴포넌트의 스케일된 Extent, 즉 x/y/z 방향으로 반지름을 구함
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	// 박스 중심 위치
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	// 각 축별로 -Extent ~ +Extent 범위 내에서 무작위 좌표를 생성
	FVector RandomPoint = BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);

	// 바닥 감지를 위한 LineTrace
	FHitResult HitResult;
	FVector TraceStart = RandomPoint + FVector(0.f, 0.f, 500.f);
	FVector TraceEnd = RandomPoint - FVector(0.f, 0.f, 1000.f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// 바닥 찾기
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		// 바닥 위 50 유닛 높이에 스폰
		return HitResult.Location + FVector(0.f, 0.f, 50.f);
	}

	// 바닥을 찾지 못한 경우 원래 위치 반환
	return RandomPoint;
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable)
		return nullptr;

	// 모든 Row 가져오기
	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext"));
	ItemDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.IsEmpty())
		return nullptr;

	// 전체 확률 합 구하기
	float TotalChance = 0.f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;
		}
	}

	const float RandValue = FMath::FRandRange(0.f, TotalChance);
	float AccumulateChance = 0.f;

	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->SpawnChance;
		if (RandValue <= AccumulateChance)
		{
			return Row;
		}
	}
	return nullptr;
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass)
		return nullptr;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomPointInVolume(),
		FRotator::ZeroRotator);

	return SpawnedActor;
}
