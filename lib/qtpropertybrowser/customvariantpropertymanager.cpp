#include "customvariantpropertymanager.h"

CustomVariantPropertyManager::CustomVariantPropertyManager(QWidget* pParent)
    : QtVariantPropertyManager(pParent)
{
}

QtVariantProperty* CustomVariantPropertyManager::addProperty(int id, int type, const QString& name)
{
    if (contains(type))
        return nullptr;
    QtVariantProperty* pProperty = QtVariantPropertyManager::addProperty(type, name);
    registerProperty(pProperty, id);
    return pProperty;
}

//! Clear all the properties
void CustomVariantPropertyManager::clear()
{
    mPropertyToID.clear();
    mIDToProperty.clear();
    QtVariantPropertyManager::clear();
}

//! Check if the editor contains the property
bool CustomVariantPropertyManager::contains(QtProperty* pProperty) const
{
    return mPropertyToID.contains(pProperty);
}

//! Check if there is the property associated with the specified identifier
bool CustomVariantPropertyManager::contains(int id) const
{
    return mIDToProperty.contains(id);
}

//! Retrieve the property identifier
int CustomVariantPropertyManager::id(QtProperty* pProperty) const
{
    if (mPropertyToID.contains(pProperty))
        return mPropertyToID[pProperty];
    else
        return -1;
}

//! Register the property in the tree structure
bool CustomVariantPropertyManager::registerProperty(QtProperty* pProperty, int id)
{
    if (contains(id))
        return false;
    mPropertyToID[pProperty] = id;
    mIDToProperty[id] = pProperty;
    return true;
}

//! Unregister the property from the tree structure
bool CustomVariantPropertyManager::unregisterProperty(int id)
{
    if (!contains(id))
        return false;
    QtProperty* pProperty = mIDToProperty[id];
    mPropertyToID.remove(pProperty);
    mIDToProperty.remove(id);
    return true;
}
