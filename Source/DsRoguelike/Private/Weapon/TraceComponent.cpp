#include "Weapon/TraceComponent.h"

UTraceComponent::UTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
#if WITH_EDITOR
	bVisualizeComponent = true;
#endif
}
