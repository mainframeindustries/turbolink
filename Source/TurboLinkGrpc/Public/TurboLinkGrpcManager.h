#pragma once

#include "CoreMinimal.h"
#include "TurboLinkGrpcClient.h"
#include "TurboLinkGrpcManager.generated.h"

class UGrpcService;
class GrpcContext;

UCLASS(ClassGroup=TurboLink)
class TURBOLINKGRPC_API UTurboLinkGrpcManager : public UObject
{
	GENERATED_BODY()

public:
	void InitManager();
	void Shutdown();
	void Tick(float DeltaTime);

	void* GetNextTag(TSharedPtr<GrpcContext> Context);
	void RemoveTag(void* Tag);

	FGrpcContextHandle GetNextContextHandle();

	void SetAuthToken(const FString Token);
public:
	class Private;
	Private* const d=nullptr;

public:
	UFUNCTION(BlueprintCallable)
	UGrpcService* MakeService(const FString& ServiceName);

	UFUNCTION(BlueprintCallable)
	void ReleaseService(UGrpcService* Service);

protected:
	UPROPERTY()
	TMap<FString, UGrpcService*> WorkingService;

	UPROPERTY()
	TSet<UGrpcService*> ShutingDownService;

private:
	TMap<FString, UClass*> ServiceClassMap;

	TMap<void*, TSharedPtr<GrpcContext>> GrpcContextMap;
	uint64_t NextTag = 0;

	uint32 NextContextHandle = 0;

	bool bIsInitialized = false;
	bool bIsShutdowning = false;

public:
	UTurboLinkGrpcManager();
	virtual ~UTurboLinkGrpcManager();
};