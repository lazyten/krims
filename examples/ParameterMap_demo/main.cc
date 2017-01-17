//
// Copyright (C) 2016 by the krims authors
//
// This file is part of krims.
//
// krims is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// krims is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with krims. If not, see <http://www.gnu.org/licenses/>.
//

#include <iomanip>
#include <iostream>
#include <krims/ParameterMap.hh>
#include <krims/version.hh>

using namespace krims;

// Some boring class.
struct A {
  int data = 15;
};

ParameterMap make_map() {
  // Make a pointer to such a boring class
  auto aptr = std::make_shared<A>();

  // Make a parameter map using a list of key,value pairs.
  // As values we can use:
  //      - integer and floating point types
  //      - strings
  //      - All classes deriving off ::krims::Subscribable
  //        (i.e. which are savely subscribable)
  //      - std::shared_ptr of whatever
  //
  // This one the one hand avoids expensive copies, but on the other hand
  // makes sure that the referenced object actually exist until the map
  // is no longer needed.
  return {{"blubber", 1},
          {"eins", "zwei"},
          {"pi", 3.141592},
          {"a", aptr},
          {"always", "never"}};

  // E.g. in this case the object referenced by *aptr will remain alive until
  // the ParameterMap is destroyed, since this is guaranteed by the managed
  // std::shared_ptr
}

void print_map(const ParameterMap& map) {
  // Print value of key blubber, which is an integer.
  // If blubber is not in the map, the default value 0 is chosen
  std::cout << "blubber:          " << map.at("blubber", 0) << std::endl;

  // Print an always existing key
  std::cout << "always:           " << map.at<std::string>("always") << std::endl;
  std::cout << "sub/always:       " << map.at<std::string>("sub/always", "notfound")
            << std::endl;

  // Print some others
  std::cout << "pi:               " << map.at<double>("pi") << std::endl;
  std::cout << "eins:             " << map.at<std::string>("eins") << std::endl;
  std::cout << "a->data:          " << map.at<A>("a").data << std::endl;

#ifndef DEBUG
  // Note that the types have to match. This will yield undefined behaviour
  // in a Release build and an aborting of the Program in a Debug build.
  std::cout << "always (rubbish): " << map.at<int>("always") << std::endl;
#endif

  std::cout << std::endl;
}

void print_keys(const ParameterMap& map) {
  // Print all keys which are stored along
  // with a string describing the type of the data.
  // Note: In RELEASE the type information is
  // not stored for perfomance and space reasons
  // and hence is not available!
  for (auto itkey = map.begin_keys(); itkey != map.end_keys(); ++itkey) {
    std::cout << std::setw(14) << *itkey << "  " << map.type_name_of(*itkey) << std::endl;
  }
}

void modify_map(ParameterMap& map) {
  // Remove an element.
  map.erase("blubber");

  // Modify the value of another
  map.update("pi", 3.0);  // roughly ...

  // Modify the object using the reference returned
  // by the at function.
  map.at<std::string>("eins") += "111";

  // Get the value of the object behind the key "a" as a pointer
  auto aptr = map.at_ptr<A>("a");

  // And modify the data:
  aptr->data = 9;

#ifndef DEBUG
  // Note: The pointer one receives this way is *not* a shared pointer, but
  // a wrapper object called RCPWrapper, that does the right thing for any
  // kind of data passed to the map, i.e. also Subscribable objects passed
  // to the ParameterMap by reference can retrieved and used like *pointers*
  // this way.
  //
  // One can make a proper shared pointer out of this RCPWrapper by explicit casting:
  std::shared_ptr<A> asptr = static_cast<std::shared_ptr<A>>(aptr);

  // In Debug mode this will abort the program for Subscriptions
  // (but of cause it will be fine for objects owned by the ParameterMap).
  // In Release mode a subscribed object will be *copied* and a pointer
  // to the copy returned.
  //
  // Whether a full shared_ptr is a available, i.e. whether the ParameterMap
  // actually has ownership of the data, can be checked using
  // bool is_shared = aptr.is_shared_ptr();

  // asptr is now a std::shared_ptr:
  if (asptr.unique()) return;
#endif  // DEBUG
}

void modify_map_other(ParameterMap& map) {
  // Add/update some other values and things:
  map.update({{"blubber", 99},
              {"always", "sure"},
              {"pi", -10.},
              {"eins", "11111111"},
              {"a", std::make_shared<A>()}});
}

int main() {
  std::cout << "Using krims version " << krims::version::version_string() << std::endl
            << std::endl;

  ParameterMap map = make_map();

  std::cout << "Printing map" << std::endl;
  print_map(map);

  std::cout << "#" << std::endl;
  std::cout << "# Modify submap with modify_map_other(map.submap(\"sub\")" << std::endl;
  std::cout << "#" << std::endl;

  ParameterMap submap = map.submap("sub");
  modify_map_other(submap);

  std::cout << "Printing all keys of submap:" << std::endl;
  print_keys(map.submap("sub"));

  std::cout << "Printing submap:" << std::endl;
  print_map(map.submap("sub"));

  std::cout << "#" << std::endl;
  std::cout << "# Modify map with modify_map(map)" << std::endl;
  std::cout << "#" << std::endl;

  modify_map(map);

  std::cout << "Printing all keys of map" << std::endl;
  print_keys(map);
  std::cout << "Printing map" << std::endl;
  print_map(map);
  std::cout << "Printing submap:" << std::endl;
  print_map(map.submap("sub"));
  return 0;
}
