// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = true;

	// 루트 컴포넌트 생성 및 설정
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	// 충돌 컴포넌트 생성 및 설정
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	// 겹침만 감지하는 프로파일 설정
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	// 루트 컴포넌트에 부착
	Collision->SetupAttachment(Scene);

	// 스태틱 메시 컴포넌트 생성 및 설정
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Collision);
	// 메시가 불필요하게 충돌을 막지 않도록 비활성화
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Overlap 이벤트 바인딩
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnItemOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnItemEndOverlap);

	// 기본 회전 속도 (초당 90도)
	RotationSpeed = 90.f;
}

void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Y축(Yaw)을 기준으로 회전
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += RotationSpeed * DeltaTime;
	SetActorRotation(NewRotation);
}

void ABaseItem::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// OtherActor가 유효하고 플레이어인지 확인
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		// 아이템 사용 (획득) 로직 호출
		ActivateItem(OtherActor);
	}
}

void ABaseItem::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ABaseItem::ActivateItem(AActor* Activator)
{
	UParticleSystemComponent* Particle = nullptr;
	UAudioComponent* AudioComp = nullptr;

	if (PickupParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			PickupParticle,
			GetActorLocation(),
			GetActorRotation(),
			true);
		
		// 파티클이 2초 후 자동으로 비활성화되도록 설정
		if (Particle)
		{
			Particle->bAutoDestroy = true;
			Particle->CustomTimeDilation = 2.0f;
		}
	}

	if (PickupSound)
	{
		AudioComp = UGameplayStatics::SpawnSoundAtLocation(
			GetWorld(),
			PickupSound,
			GetActorLocation());
	}

	// 파티클 타이머
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
					WeakParticle->Deactivate();
					WeakParticle->DestroyComponent();
				}
			},
			2.0f,
			false);
	}

	// 사운드 타이머
	if (AudioComp)
	{
		FTimerHandle StopSoundTimerHandle;
		TWeakObjectPtr<UAudioComponent> WeakAudio = AudioComp;

		GetWorld()->GetTimerManager().SetTimer(
			StopSoundTimerHandle,
			[WeakAudio]()
			{
				if (WeakAudio.IsValid())
				{
					WeakAudio->Stop();
					WeakAudio->DestroyComponent();
				}
			},
			2.0f,
			false);
	}
}

FName ABaseItem::GetItemType() const
{
	return ItemType;
}

void ABaseItem::DestroyItem()
{
	Destroy();
}
