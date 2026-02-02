// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "CoinItem.generated.h"

UCLASS()
class SPARTAPROJECT_API ACoinItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ACoinItem();

protected:
	// 코인 획득 시 얻을 점수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 PointValue;

	virtual void ActivateItem(AActor* Activator) override;
};
