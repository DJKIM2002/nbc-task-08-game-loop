// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnRow.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class SPARTAPROJECT_API ASpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	ASpawnVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	TObjectPtr<USceneComponent> Scene;
	// 스폰 영역을 정의하는 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	TObjectPtr<UBoxComponent> SpawningBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TObjectPtr<UDataTable> ItemDataTable;

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnRandomItem();
	FItemSpawnRow* GetRandomItem() const;
	// 특정 아이템 클래스를 스폰하는 함수
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass);
	// 스폰 볼륨 내부에서 무작위 좌표를 얻어오는 함수
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	FVector GetRandomPointInVolume() const;
};
