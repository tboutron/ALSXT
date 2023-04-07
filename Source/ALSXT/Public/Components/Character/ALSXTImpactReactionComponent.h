// MIT

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Utility/ALSXTStructs.h"
#include "ALSXTCharacter.h"
#include "AlsCharacter.h"
#include "Chaos/ChaosEngineInterface.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h" 
#include "Settings/ALSXTImpactReactionSettings.h"
#include "State/ALSXTImpactReactionState.h" 
#include "Components/TimelineComponent.h"
#include "ALSXTImpactReactionComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=(Physics), meta=(BlueprintSpawnableComponent) )
class ALSXT_API UALSXTImpactReactionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UALSXTImpactReactionComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Parameters")
	UPARAM(meta = (Categories = "Als.Impact Form")) FGameplayTag GetCurrentBumpForm();

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AALSXTCharacter* Character{ Cast<AALSXTCharacter>(GetOwner()) };
	AAlsCharacter* AlsCharacter{ Cast<AAlsCharacter>(GetOwner()) };
	FALSXTImpactReactionParameters ImpactReactionParameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (AllowPrivateAccess))
	FALSXTGeneralImpactReactionSettings ImpactReactionSettings;

	void ObstacleTrace();

	UFUNCTION(BlueprintCallable, Category = "ALS|Movement System")
	const FALSXTImpactReactionState& GetImpactReactionState() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character", Meta = (AutoCreateRefTerm = "NewImpactReactionState"))
	void SetImpactReactionState(const FALSXTImpactReactionState& NewImpactReactionState);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "ALS|Als Character", Meta = (AutoCreateRefTerm = "NewImpactReactionState"))
	FALSXTImpactReactionState ProcessNewImpactReactionState(const FALSXTImpactReactionState& NewImpactReactionState);

	UFUNCTION(Server, Unreliable)
	void ServerProcessNewImpactReactionState(const FALSXTImpactReactionState& NewImpactReactionState);

private:
	UFUNCTION(Server, Unreliable)
	void ServerSetImpactReactionState(const FALSXTImpactReactionState& NewImpactReactionState);

	UFUNCTION()
	void OnReplicate_ImpactReactionState(const FALSXTImpactReactionState& PreviousImpactReactionState);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool ShouldRecieveVelocityDamage();

	UFUNCTION(BlueprintCallable, Category = "Vitals")
	float GetBaseVelocityDamage();

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnImpactReactionStateChanged(const FALSXTImpactReactionState& PreviousImpactReactionState);

	// Settings
private:
	bool IsImpactReactionAllowedToStart(const UAnimMontage* Montage) const;

protected:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Settings")
	bool ShouldSpawnParticleActor(FDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Settings")
	bool ShouldSpawnParticleActorModeration(FDoubleHitResult Hit);

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Settings")
	bool CanReact();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Settings")
	bool CanPerformAnticipationReaction();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Settings")
	bool CanPerformDefensiveReaction();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Settings")
	bool CanGraspImpactPoint();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Settings")
	bool ShouldPerformAnticipationReaction();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Settings")
	bool ShouldPerformDefensiveReaction();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Settings")
	bool ShouldGraspImpactPoint();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Settings")
	bool ShouldFall();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Settings")
	bool ShouldPerformResponse();

	// Parameters
private:
	UFUNCTION()
	void ImpactTimelineUpdate(float Value);

	FTimeline ImpactTimeline;

	FTimerHandle TimeSinceLastRecoveryTimerHandle;
	float TimeSinceLastRecovery;

	FTimerHandle TimeSinceLastResponseTimerHandle;
	float TimeSinceLastResponse;

	FImpactReactionAnimation LastImpactReactionAnimation;
	FAttackReactionAnimation LastAttackReactionAnimation;
	FSyncedAttackAnimation LastSyncedAttackReactionAnimation;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	FBumpReactionAnimation SelectBumpReactionMontage(const FGameplayTag& Velocity, const FGameplayTag& Side, const FGameplayTag& Form);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	FBumpReactionAnimation SelectCrowdNavigationReactionMontage(const FGameplayTag& Velocity, const FGameplayTag& Side, const FGameplayTag& Form);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	FAnticipationPose SelectAnticipationMontage(const FGameplayTag& Strength, const FGameplayTag& Side, const FGameplayTag& Form, const FGameplayTag& Health);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	FAnticipationPose SelectDefensiveMontage(const FGameplayTag& Strength, const FGameplayTag& Side, const FGameplayTag& Form, const FGameplayTag& Health);	

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	FAttackReactionAnimation SelectAttackReactionMontage(FAttackDoubleHitResult Hit);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	FImpactReactionAnimation SelectImpactReactionMontage(FDoubleHitResult Hit);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	FSyncedAttackAnimation GetSyncedMontage(int Index);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	FImpactReactionAnimation SelectClaspImpactPointMontage(FDoubleHitResult Hit);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	FAnticipationPose SelectSteadyMontage(const FGameplayTag& Side);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	FActionMontageInfo SelectImpactFallMontage(FDoubleHitResult Hit);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	FActionMontageInfo SelectAttackFallMontage(FDoubleHitResult Hit);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	UAnimMontage* SelectImpactFallenPose(FDoubleHitResult Hit);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	UAnimMontage* SelectAttackFallenPose(FDoubleHitResult Hit);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	FActionMontageInfo SelectImpactGetUpMontage(FDoubleHitResult Hit);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	FActionMontageInfo SelectAttackGetUpMontage(FDoubleHitResult Hit);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	FResponseAnimation SelectImpactResponseMontage(FAttackDoubleHitResult Hit);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	FResponseAnimation SelectAttackResponseMontage(FAttackDoubleHitResult Hit);

	UFUNCTION(BlueprintNativeEvent, Category = "Parameters")
	UALSXTImpactReactionSettings* SelectImpactReactionSettings();

public:
	UPROPERTY(EditAnywhere, Category = "Impact Reaction Timeline")
	UCurveFloat* CurveFloat;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Vitals")
	float GetHealth();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Vitals")
	float GetStamina();

	UFUNCTION(BlueprintCallable, Category = "Parameters")
	FGameplayTag GetCharacterVelocity();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Parameters")
	UNiagaraSystem* GetImpactReactionParticle(FDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Parameters")
	UNiagaraSystem* GetBodyFallParticle(FDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Parameters")
	TSubclassOf<AActor> GetImpactReactionParticleActor(FDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Parameters")
	FALSXTCharacterSound GetImpactReactionSound(FDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Parameters")
	FALSXTCharacterSound GetAttackReactionSound(FDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Parameters")
	FALSXTCharacterSound GetBodyFallSound(FDoubleHitResult Hit);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FALSXTImpactReactionState ImpactReactionState;

	// Entry Events
public:
	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void AnticipationReaction(const FGameplayTag& Velocity, const FGameplayTag& Side, const FGameplayTag& Form, FVector AnticipationPoint);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void SyncedAnticipationReaction(FVector AnticipationPoint);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void DefensiveReaction(const FGameplayTag& Velocity, const FGameplayTag& Side, const FGameplayTag& Form, FVector AnticipationPoint);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void CrowdNavigationReaction(const FGameplayTag& Gait, const FGameplayTag& Side, const FGameplayTag& Form);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void BumpReaction(const FGameplayTag& Gait, const FGameplayTag& Side, const FGameplayTag& Form);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void ImpactReaction(FDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void AttackReaction(FAttackDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void SyncedAttackReaction(int Index);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void ClutchImpactPoint(FDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void BumpFall();
	
	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void ImpactFall(FDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void AttackFall(FAttackDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void SyncedAttackFall(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void BraceForImpact();

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void ImpactGetUp(FDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void AttackGetUp(FAttackDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void SyncedAttackGetUp(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void ImpactResponse(FDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void AttackResponse(FAttackDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, Category = "Impact Reaction")
	void BodyFallReaction(FAttackDoubleHitResult Hit);

private:

	void StartBumpReaction(const FGameplayTag& Gait, const FGameplayTag& Side, const FGameplayTag& Form);

	void StartCrowdNavigationReaction(const FGameplayTag& Gait, const FGameplayTag& Side, const FGameplayTag& Form);

	void StartAnticipationReaction(const FGameplayTag& Velocity, const FGameplayTag& Side, const FGameplayTag& Form, FVector AnticipationPoint);

	void StartDefensiveReaction(const FGameplayTag& Velocity, const FGameplayTag& Side, const FGameplayTag& Form, FVector AnticipationPoint);

	void StartImpactReaction(FDoubleHitResult Hit);
	
	void StartAttackReaction(FAttackDoubleHitResult Hit);

	void StartSyncedAttackReaction(int32 Index);

	void StartClutchImpactPoint(FDoubleHitResult Hit);

	void StartImpactFall(FDoubleHitResult Hit);

	void StartAttackFall(FAttackDoubleHitResult Hit);

	void StartSyncedAttackFall(int32 Index);

	void StartBraceForImpact();

	void StartImpactFallLand(FDoubleHitResult Hit);

	void StartAttackFallLand(FAttackDoubleHitResult Hit);

	void StartSyncedAttackFallLand(int32 Index);

	void StartImpactGetUp(FDoubleHitResult Hit);

	void StartAttackGetUp(FAttackDoubleHitResult Hit);

	void StartSyncedAttackGetUp(int32 Index);

	void StartImpactResponse(FDoubleHitResult Hit);

	void StartAttackResponse(FAttackDoubleHitResult Hit);

	// RPCs

	UFUNCTION(Server, Reliable)
	void ServerBumpReaction(const FGameplayTag& Gait, const FGameplayTag& Side, const FGameplayTag& Form);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastBumpReaction(const FGameplayTag& Gait, const FGameplayTag& Side, const FGameplayTag& Form);

	UFUNCTION(Server, Reliable)
	void ServerCrowdNavigationReaction(const FGameplayTag& Gait, const FGameplayTag& Side, const FGameplayTag& Form);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastCrowdNavigationReaction(const FGameplayTag& Gait, const FGameplayTag& Side, const FGameplayTag& Form);

	UFUNCTION(Server, Reliable)
	void ServerAnticipationReaction(const FGameplayTag& Velocity, const FGameplayTag& Side, const FGameplayTag& Form, FVector AnticipationPoint);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAnticipationReaction(const FGameplayTag& Velocity, const FGameplayTag& Side, const FGameplayTag& Form, FVector AnticipationPoint);

	UFUNCTION(Server, Reliable)
	void ServerDefensiveReaction(const FGameplayTag& Velocity, const FGameplayTag& Side, const FGameplayTag& Form, FVector AnticipationPoint);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDefensiveReaction(const FGameplayTag& Velocity, const FGameplayTag& Side, const FGameplayTag& Form, FVector AnticipationPoint);

	UFUNCTION(Server, Reliable)
	void ServerImpactReaction(FDoubleHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastImpactReaction(FDoubleHitResult Hit);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttackReaction(FAttackDoubleHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttackReaction(FAttackDoubleHitResult Hit);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSyncedAttackReaction(int32 Index);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSyncedAttackReaction(int32 Index);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerClutchImpactPoint(FDoubleHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastClutchImpactPoint(FDoubleHitResult Hit);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerImpactFall(FDoubleHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastImpactFall(FDoubleHitResult Hit);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttackFall(FAttackDoubleHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttackFall(FAttackDoubleHitResult Hit);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSyncedAttackFall(int32 Index);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSyncedAttackFall(int32 Index);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBraceForImpact();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastBraceForImpact();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerImpactFallLand(FDoubleHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastImpactFallLand(FDoubleHitResult Hit);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttackFallLand(FAttackDoubleHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttackFallLand(FAttackDoubleHitResult Hit);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSyncedAttackFallLand(int32 Index);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSyncedAttackFallLand(int32 Index);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerImpactGetUp(FDoubleHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastImpactGetUp(FDoubleHitResult Hit);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttackGetUp(FAttackDoubleHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttackGetUp(FAttackDoubleHitResult Hit);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSyncedAttackGetUp(int32 Index);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSyncedAttackGetUp(int32 Index);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerImpactResponse(FDoubleHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastImpactResponse(FDoubleHitResult Hit);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttackResponse(FAttackDoubleHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttackResponse(FAttackDoubleHitResult Hit);

	// Start RPCs

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartBumpReaction(FDoubleHitResult Hit, FActionMontageInfo Montage, TSubclassOf<AActor> ParticleActor, UNiagaraSystem* Particle, USoundBase* Audio);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartBumpReaction(FDoubleHitResult Hit, FActionMontageInfo Montage, TSubclassOf<AActor> ParticleActor, UNiagaraSystem* Particle, USoundBase* Audio);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartCrowdNavigationReaction(FDoubleHitResult Hit, FActionMontageInfo Montage, TSubclassOf<AActor> ParticleActor, UNiagaraSystem* Particle, USoundBase* Audio);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartCrowdNavigationReaction(FDoubleHitResult Hit, FActionMontageInfo Montage, TSubclassOf<AActor> ParticleActor, UNiagaraSystem* Particle, USoundBase* Audio);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartAnticipationReaction(FActionMontageInfo Montage, FVector AnticipationPoint);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartAnticipationReaction(FActionMontageInfo Montage, FVector AnticipationPoint);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartDefensiveReaction(FActionMontageInfo Montage, USoundBase* Audio, FVector AnticipationPoint);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartDefensiveReaction(FActionMontageInfo Montage, USoundBase* Audio, FVector AnticipationPoint);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartImpactReaction(FDoubleHitResult Hit, UAnimMontage* Montage, TSubclassOf<AActor> ParticleActor, UNiagaraSystem* Particle, USoundBase* Audio);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartImpactReaction(FDoubleHitResult Hit, UAnimMontage* Montage, TSubclassOf<AActor> ParticleActor, UNiagaraSystem* Particle, USoundBase* Audio);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartAttackReaction(FAttackDoubleHitResult Hit, FActionMontageInfo Montage, TSubclassOf<AActor> ParticleActor, UNiagaraSystem* Particle, USoundBase* Audio);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartAttackReaction(FAttackDoubleHitResult Hit, FActionMontageInfo Montage, TSubclassOf<AActor> ParticleActor, UNiagaraSystem* Particle, USoundBase* Audio);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartSyncedAttackReaction(FActionMontageInfo Montage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartSyncedAttackReaction(FActionMontageInfo Montage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartClutchImpactPoint(UAnimMontage* Montage, FVector ImpactPoint);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartClutchImpactPoint(UAnimMontage* Montage, FVector ImpactPoint);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartImpactFall(FDoubleHitResult Hit, FActionMontageInfo Montage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartImpactFall(FDoubleHitResult Hit, FActionMontageInfo Montage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartAttackFall(FAttackDoubleHitResult Hit, FActionMontageInfo Montage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartAttackFall(FAttackDoubleHitResult Hit, FActionMontageInfo Montage);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartSyncedAttackFall(FActionMontageInfo Montage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartSyncedAttackFall(FActionMontageInfo Montage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartBraceForImpact(UAnimMontage* Montage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartBraceForImpact(UAnimMontage* Montage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartImpactFallLand(FDoubleHitResult Hit, FActionMontageInfo Montage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartImpactFallLand(FDoubleHitResult Hit, FActionMontageInfo Montage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartAttackFallLand(FAttackDoubleHitResult Hit, FActionMontageInfo Montage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartAttackFallLand(FAttackDoubleHitResult Hit, FActionMontageInfo Montage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartSyncedAttackFallLand(FActionMontageInfo Montage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartSyncedAttackFallLand(FActionMontageInfo Montage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartImpactGetUp(FDoubleHitResult Hit, FActionMontageInfo Montage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartImpactGetUp(FDoubleHitResult Hit, FActionMontageInfo Montage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartAttackGetUp(FAttackDoubleHitResult Hit, FActionMontageInfo Montage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartAttackGetUp(FAttackDoubleHitResult Hit, FActionMontageInfo Montage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartSyncedAttackGetUp(FActionMontageInfo Montage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartSyncedAttackGetUp(FActionMontageInfo Montage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartImpactResponse(FDoubleHitResult Hit, FActionMontageInfo Montage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartImpactResponse(FDoubleHitResult Hit, FActionMontageInfo Montage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartAttackResponse(FAttackDoubleHitResult Hit, FActionMontageInfo Montage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartAttackResponse(FAttackDoubleHitResult Hit, FActionMontageInfo Montage);

	// Particle Actor RPCs

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnParticleActor(FDoubleHitResult Hit, TSubclassOf<AActor> ParticleActor);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnParticleActor(FDoubleHitResult Hit, TSubclassOf<AActor> ParticleActor);

	// Implementations

	void StartAnticipationReactionImplementation(FActionMontageInfo Montage, FVector AnticipationPoint);

	void StartDefensiveReactionImplementation(FActionMontageInfo Montage, USoundBase* Audio, FVector AnticipationPoint);

	void StartCrowdNavigationReactionImplementation(FDoubleHitResult Hit, FActionMontageInfo Montage, TSubclassOf<AActor> ParticleActor, UNiagaraSystem* Particle, USoundBase* Audio);

	void StartBumpReactionImplementation(FDoubleHitResult Hit, FActionMontageInfo Montage, TSubclassOf<AActor> ParticleActor, UNiagaraSystem* Particle, USoundBase* Audio);

	void StartImpactReactionImplementation(FDoubleHitResult Hit, UAnimMontage* Montage, TSubclassOf<AActor> ParticleActor, UNiagaraSystem* Particle, USoundBase* Audio);

	void StartAttackReactionImplementation(FAttackDoubleHitResult Hit, FActionMontageInfo Montage, TSubclassOf<AActor> ParticleActor, UNiagaraSystem* Particle, USoundBase* Audio);

	void StartSyncedAttackReactionImplementation(FActionMontageInfo Montage);

	void StartClutchImpactPointImplementation(UAnimMontage* Montage, FVector ImpactPoint);

	void StartImpactFallImplementation(FDoubleHitResult Hit, FActionMontageInfo Montage);

	void StartAttackFallImplementation(FAttackDoubleHitResult Hit, FActionMontageInfo Montage);

	void StartSyncedAttackFallImplementation(FActionMontageInfo Montage);

	void StartBraceForImpactImplementation(UAnimMontage* Montage);

	void StartImpactFallLandImplementation(FDoubleHitResult Hit, FActionMontageInfo Montage);

	void StartAttackFallLandImplementation(FAttackDoubleHitResult Hit, FActionMontageInfo Montage);

	void StartSyncedAttackFallLandImplementation(FActionMontageInfo Montage);

	void StartImpactGetUpImplementation(FDoubleHitResult Hit, FActionMontageInfo Montage);

	void StartAttackGetUpImplementation(FAttackDoubleHitResult Hit, FActionMontageInfo Montage);

	void StartSyncedAttackGetUpImplementation(FActionMontageInfo Montage);

	void StartImpactResponseImplementation(FDoubleHitResult Hit, FActionMontageInfo Montage);

	void StartAttackResponseImplementation(FAttackDoubleHitResult Hit, FActionMontageInfo Montage);

	void SpawnParticleActorImplementation(FDoubleHitResult Hit, TSubclassOf<AActor> ParticleActor);

	void RefreshImpactReaction(float DeltaTime);

	void RefreshImpactReactionPhysics(float DeltaTime);

public:
	UFUNCTION(BlueprintCallable, Category = "Hooks")
	void StopImpactReaction();

protected:
	// Hooks 

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hooks")
	void OnSpawnParticleActor(const FDoubleHitResult& Hit);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hooks")
	void OnAttackReactionStarted(FAttackDoubleHitResult Hit);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hooks")
	void OnImpactReactionStarted(FDoubleHitResult Hit);

	UFUNCTION(BlueprintNativeEvent, Category = "Hooks")
	void OnImpactReactionEnded();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hooks")
	void OnSyncedReactionStarted();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hooks")
	void OnSyncedReactionEnded();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hooks")
	void OnFallStarted();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hooks")
	void OnFallLand();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hooks")
	void OnGetUpStarted();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hooks")
	void OnGetUpEnded();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hooks")
	void OnResponseStarted();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hooks")
	void OnResponseEnded();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hooks")
	void OnABumpReactionFinished(const FGameplayTag& Gait, const FGameplayTag& Side, const FGameplayTag& Form);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hooks")
	void OnAttackReactionFinished(FAttackDoubleHitResult Hit, const FGameplayTag& AttackMode);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hooks")
	void OnImpactReactionFinished(FDoubleHitResult Hit);

};

inline const FALSXTImpactReactionState& UALSXTImpactReactionComponent::GetImpactReactionState() const
{
	return ImpactReactionState;
}