// Fill out your copyright notice in the Description page of Project Settings.

#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AMineItem::AMineItem()
{
	ExplosionDelay = 5.f;
	ExplosionRadius = 300.f;
	ExplosionDamage = 30;
	ItemType = "Mine";

	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->InitSphereRadius(ExplosionRadius);
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollision->SetupAttachment(Scene);
}

void AMineItem::ActivateItem(AActor* Activator)
{
	// 5초 후 폭발 실행
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AMineItem::Explode, ExplosionDelay);
}

void AMineItem::Explode()
{
	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			// 데미지를 발생시켜 Actor->TakeDamage()가 실행되도록 함
			UGameplayStatics::ApplyDamage(
				Actor,
				ExplosionDamage,
				nullptr, // 데미지를 유발한 주체 (지뢰를 설치한 캐릭터가 없으므로 nullptr)
				this,	 // 데미지를 준 액터 (지뢰 자신)
				UDamageType::StaticClass()); // 기본 데미지 타입
		}
	}
	DestroyItem();
}
