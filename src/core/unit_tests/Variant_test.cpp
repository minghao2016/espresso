/*
  Copyright (C) 2016,2017 The ESPResSo project

  This file is part of ESPResSo.

  ESPResSo is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ESPResSo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define BOOST_TEST_MODULE Variant test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "script_interface/Variant.hpp"
using namespace ScriptInterface;

/* Check that the enum and the types are in order. */
BOOST_AUTO_TEST_CASE(variant_types) {
  BOOST_CHECK(static_cast<int>(VariantType::BOOL) == Variant(bool{}).which());
  BOOST_CHECK(static_cast<int>(VariantType::INT) == Variant(int{}).which());
  BOOST_CHECK(static_cast<int>(VariantType::DOUBLE) ==
              Variant(double{}).which());
  BOOST_CHECK(static_cast<int>(VariantType::STRING) ==
              Variant(std::string{}).which());
  BOOST_CHECK(static_cast<int>(VariantType::INT_VECTOR) ==
              Variant(std::vector<int>{}).which());
  BOOST_CHECK(static_cast<double>(VariantType::DOUBLE_VECTOR) ==
              Variant(std::vector<double>{}).which());
  BOOST_CHECK(static_cast<int>(VariantType::OBJECTID) ==
              Variant(ObjectId{}).which());
  BOOST_CHECK(static_cast<int>(VariantType::VECTOR) ==
              Variant(std::vector<Variant>{}).which());
}

BOOST_AUTO_TEST_CASE(infer_type_test) {
  static_assert(infer_type<bool>() == VariantType::BOOL, "");
  static_assert(infer_type<int>() == VariantType::INT, "");
  static_assert(infer_type<double>() == VariantType::DOUBLE, "");
  static_assert(infer_type<ObjectId>() == VariantType::OBJECTID, "");
  static_assert(infer_type<std::string>() == VariantType::STRING, "");
  static_assert(infer_type<std::vector<int>>() == VariantType::INT_VECTOR, "");
  static_assert(infer_type<std::vector<double>>() == VariantType::DOUBLE_VECTOR,
                "");
  static_assert(infer_type<std::vector<Variant>>() == VariantType::VECTOR, "");
}

BOOST_AUTO_TEST_CASE(is_a) {
  BOOST_CHECK(is_bool(Variant(bool{})));
  BOOST_CHECK(is_int(Variant(int{})));
  BOOST_CHECK(is_double(Variant(double{})));
  BOOST_CHECK(is_string(Variant(std::string{})));
  BOOST_CHECK(is_int_vector(Variant(std::vector<int>{})));
  BOOST_CHECK(is_double_vector(Variant(std::vector<double>{})));
  BOOST_CHECK(is_objectid(Variant(ObjectId{})));
  BOOST_CHECK(is_vector(Variant(std::vector<Variant>{})));
}

BOOST_AUTO_TEST_CASE(transform_vectors_test) {
  std::vector<Variant> vv;

  vv.emplace_back(std::vector<Variant>{{1, 2, 3}});
  vv.emplace_back(std::vector<Variant>{
      std::string("test"), std::vector<Variant>{1.1, 1.2, 1.3, 1.4}});

  /* v = {{INT, INT, INT}, {STRING, {DOUBLE, DOUBLE, DOUBLE, DOUBLE}}} */
  auto v = Variant(vv);

  transform_vectors(v);
  /* v should now be { INT_VECTOR, { STRING, DOUBLE_VECTOR}} */
  BOOST_CHECK(is_vector(v));

  BOOST_CHECK(boost::get<std::vector<Variant>>(v).size() == 2);
  /* First vector should be transformed to an INT_VECTOR */
  BOOST_CHECK(is_int_vector(boost::get<std::vector<Variant>>(v)[0]));
  /* The nested vector should be unchanged because it is mixed. */
  BOOST_CHECK(is_vector(boost::get<std::vector<Variant>>(v)[1]));

  auto const &inner_vv =
      boost::get<std::vector<Variant>>(boost::get<std::vector<Variant>>(v)[1]);
  BOOST_CHECK(inner_vv.size() == 2);
  /* The string should be unchanged */
  BOOST_CHECK(is_string(inner_vv[0]));
  /* The vector<Variant> should now be a DOUBLE_VECTOR */
  BOOST_CHECK(is_double_vector(inner_vv[1]));
}
