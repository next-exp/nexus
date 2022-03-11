#ifndef FACTORY_BASE_H
#define FACTORY_BASE_H

#include <G4Exception.hh>

#include <map>
#include <memory>


////////////////////////////////////////////////////////////////////////////////

template <class T>
class ObjCreatorBase
{
public:
  ObjCreatorBase() {}
  virtual ~ObjCreatorBase() {}
  virtual std::unique_ptr<T> CreateObject() = 0;
};

////////////////////////////////////////////////////////////////////////////////

template <class T>
class ObjFactory
{
public:
  // Return the (Meyers-style) singleton instance of the factory
  static ObjFactory& Instance() {
    static ObjFactory<T> f;
    return f;
  }

  //
  void RegisterObjCreator(const std::string& tag, ObjCreatorBase<T>* oc) {
    // Validate uniqueness and add to the map
    if (registry_.find(tag) != registry_.end()) {
      std::string msg =
        "Duplicated use of key '" + tag + "' registering a creator in the factory.";
      G4Exception("ObjFactory::Register()", "", FatalException, msg.c_str());
    }
    else {
      registry_[tag] = oc;
    }
  }

  //
  std::unique_ptr<T> CreateObject(const std::string& tag) {
    return registry_[tag]->CreateObject();
  }

private:
  // Constructor (hidden, since class is a singleton)
  ObjFactory() {}
  // Destructor (hidden, since class is a singleton)
  ~ObjFactory() { registry_.clear(); }

private:
  std::map<std::string, ObjCreatorBase<T>*> registry_;
};

////////////////////////////////////////////////////////////////////////////////

template <class Derived, class Base>
class ObjCreator: public ObjCreatorBase<Base>
{
public:
  // Default constructor
  ObjCreator() {}
  // Constructor for self-registration in factory
  ObjCreator(const std::string& tag) {
    ObjFactory<Base>::Instance().RegisterObjCreator(tag, this);
  }
  // Destructor
  virtual ~ObjCreator() {}
  // Return a new object instance
  std::unique_ptr<Base> CreateObject() { return std::make_unique<Derived>(); }
};

////////////////////////////////////////////////////////////////////////////////

#define REGISTER_CLASS(D,B) static ObjCreator<D,B> obj_creator(#D);

#endif
