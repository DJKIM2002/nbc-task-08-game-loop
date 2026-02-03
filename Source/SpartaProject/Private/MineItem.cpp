// Fill out your copyright notice in the Description page of Project Settings.

#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AMineItem::AMineItem()
{
	ExplosionDelay = 2.f;
	ExplosionRadius = 300.f;
	ExplosionDamage = 30;
	ItemType = "Mine";
	bHasExploded = false;

	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->InitSphereRadius(ExplosionRadius);
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollision->SetupAttachment(Scene);
}

void AMineItem::ActivateItem(AActor* Activator)
{
	if (bHasExploded)
		return;

	Super::ActivateItem(Activator);

	GetWorld()->GetTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&AMineItem::Explode,
		ExplosionDelay,
		false);

	bHasExploded = true;
}

void AMineItem::Explode()
{
	UParticleSystemComponent* Particle = nullptr;

	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle,
			GetActorLocation(),
			GetActorRotation(),
			false);
	}

  if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation());
	}

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
				nullptr,					 // 데미지를 유발한 주체 (지뢰를 설치한 캐릭터가 없으므로 nullptr)
				this,						 // 데미지를 준 액터 (지뢰 자신)
				UDamageType::StaticClass()); // 기본 데미지 타입
		}
	}
	DestroyItem();

	if (Particle)
	{
		FTimerHandle DestroyParticleTimerHandle;
		TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[WeakParticle]()
			{
				if (WeakParticle.IsValid())
				{
					WeakParticle->DestroyComponent();
				}
			},
			2.0f,
			false);
	}
}
