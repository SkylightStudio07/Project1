#include "Project1GameMode.h"
#include "Project1Character.h"
#include "UObject/ConstructorHelpers.h"

AProject1GameMode::AProject1GameMode()
{
    // set default pawn class to our Blueprinted character
    static ConstructorHelpers::FClassFinder<ACharacter> PlayerCharacterBPClass(TEXT("/Game/AnimStarterPack/Ue4ASP_Character.Ue4ASP_Character_C"));
    if (PlayerCharacterBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerCharacterBPClass.Class;
    }
}