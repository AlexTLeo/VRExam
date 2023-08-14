#pragma once 

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUI.generated.h"

UCLASS(Abstract)
class VREXAM_API UMyUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetProgressTransport(int CurrentProgress, int MaxProgress);

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UProgressBar* ProgressBar_Transport;
};

