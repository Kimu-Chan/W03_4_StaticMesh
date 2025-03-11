#pragma once
class UClass
{
public:
    const char* Name;
    UClass* ParentClass;

    UClass(const char* InName, UClass* InParent)
        : Name(InName), ParentClass(InParent) {
    }
};