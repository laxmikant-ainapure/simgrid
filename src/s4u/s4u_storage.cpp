/* Copyright (c) 2006-2017. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include "simgrid/s4u/File.hpp"
#include "simgrid/s4u/Host.hpp"
#include "simgrid/s4u/Storage.hpp"
#include "simgrid/simix.hpp"
#include "src/surf/StorageImpl.hpp"
#include <unordered_map>

namespace simgrid {
namespace xbt {
template class Extendable<simgrid::s4u::Storage>;
}

namespace s4u {

std::map<std::string, Storage*>* allStorages()
{
  std::unordered_map<std::string, surf::StorageImpl*>* map = surf::StorageImpl::storagesMap();
  std::map<std::string, Storage*>* res                     = new std::map<std::string, Storage*>;
  for (auto const& s : *map)
    res->insert({s.first, &(s.second->piface_)}); // Convert each entry into its interface

  return res;
}

Storage* Storage::byName(std::string name)
{
  surf::StorageImpl* res = surf::StorageImpl::byName(name);
  if (res == nullptr)
    return nullptr;
  return &res->piface_;
}

const std::string& Storage::getName() const
{
  return pimpl_->getName();
}

const char* Storage::getCname() const
{
  return pimpl_->getCname();
}

const char* Storage::getType()
{
  return pimpl_->typeId_.c_str();
}

Host* Storage::getHost()
{
  return attached_to_;
}

sg_size_t Storage::getSizeFree()
{
  FileSystemStorageExt* file_system = extension<FileSystemStorageExt>();

  return pimpl_->getSize() - file_system->getUsedSize();
}

sg_size_t Storage::getSizeUsed()
{
  FileSystemStorageExt* file_system = extension<FileSystemStorageExt>();
  return file_system->getUsedSize();
}

void Storage::decrUsedSize(sg_size_t size)
{
  FileSystemStorageExt* file_system = extension<FileSystemStorageExt>();
  file_system->decrUsedSize(size);
}

void Storage::incrUsedSize(sg_size_t size)
{
  FileSystemStorageExt* file_system = extension<FileSystemStorageExt>();
  file_system->incrUsedSize(size);
}

sg_size_t Storage::getSize()
{
  return pimpl_->getSize();
}

std::map<std::string, std::string>* Storage::getProperties()
{
  return simgrid::simix::kernelImmediate([this] { return pimpl_->getProperties(); });
}

const char* Storage::getProperty(std::string key)
{
  return this->pimpl_->getProperty(key);
}

void Storage::setProperty(std::string key, std::string value)
{
  simgrid::simix::kernelImmediate([this, key, value] { this->pimpl_->setProperty(key, value); });
}

std::map<std::string, sg_size_t>* Storage::getContent()
{
  FileSystemStorageExt* file_system = extension<FileSystemStorageExt>();
  return file_system->getContent();
}

sg_size_t Storage::read(sg_size_t size)
{
  return simcall_storage_read(pimpl_, size);
}

sg_size_t Storage::write(sg_size_t size)
{
  return simcall_storage_write(pimpl_, size);
}

/*************
 * Callbacks *
 *************/
simgrid::xbt::signal<void(s4u::Storage&)> Storage::onCreation;
simgrid::xbt::signal<void(s4u::Storage&)> Storage::onDestruction;

} /* namespace s4u */
} /* namespace simgrid */