#include "MyUI.h"
#include "Components/ProgressBar.h"

void UMyUI::SetProgressTransport(int CurrentProgress, int MaxProgress) {
	if (ProgressBar_Transport) {
		ProgressBar_Transport->SetPercent((float) CurrentProgress / MaxProgress);
	}
}