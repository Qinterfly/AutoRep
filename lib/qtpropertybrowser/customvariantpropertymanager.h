#ifndef CUSTOMVARIANTPROPERTYMANAGER_H
#define CUSTOMVARIANTPROPERTYMANAGER_H

#include "qtvariantproperty.h"

class CustomVariantPropertyManager : public QtVariantPropertyManager
{
public:
    CustomVariantPropertyManager(QWidget* pParent = nullptr);
    virtual ~CustomVariantPropertyManager() = default;

    QtVariantProperty* addProperty(int id, int type, const QString& name = QString());
    void clear();

    bool contains(QtProperty* pProperty) const;
    bool contains(int id) const;
    int id(QtProperty* pProperty) const;
    QtProperty* property(int id) const;

private:
    bool registerProperty(QtProperty* pProperty, int id);
    bool unregisterProperty(int id);

private:
    QMap<QtProperty*, int> mPropertyToID;
    QMap<int, QtProperty*> mIDToProperty;
};

#endif // CUSTOMVARIANTPROPERTYMANAGER_H
