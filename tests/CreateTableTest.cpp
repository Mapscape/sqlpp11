/*
 * CreateTable.cpp
 *
 * Copyright (c) 2015 Mapscape B.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <sqlpp11/declare_table.h>
#include <sqlpp11/create_table.h>
#include <sqlpp11/integral.h>
#include <sqlpp11/text.h>
#include <tuple>

#include "MockDb.h"


//************************ printing tuples *****************************
namespace aux{
template<std::size_t...> struct seq{};

template<std::size_t N, std::size_t... Is>
struct gen_seq : gen_seq<N-1, N-1, Is...>{};

template<std::size_t... Is>
struct gen_seq<0, Is...> : seq<Is...>{};

template<class Ch, class Tr, class Tuple, std::size_t... Is>
void print_tuple(std::basic_ostream<Ch,Tr>& os, Tuple const& t, seq<Is...>){
  using swallow = int[];
  (void)swallow{0, (void(os << (Is == 0? "" : ", ") << std::get<Is>(t)), 0)...};
}
} // aux::

template<class Ch, class Tr, class... Args>
auto operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<Args...> const& t)
    -> std::basic_ostream<Ch, Tr>&
{
  os << "(";
  aux::print_tuple(os, t, aux::gen_seq<sizeof...(Args)>());
  return os << ")";
}
//************************                *****************************



//template< typename... ColumnTags>
//void PrintTypes( const sqlpp::detail::type_set< ColumnTags...> &)
//{
//    std::cout << std::make_tuple( typeid( ColumnTags).name()...) << "\n";
//    std::cout << std::make_tuple( std::is_same< sqlpp::tag::primary_key, ColumnTags>::value...) << "\n";
//
//
//    //IsPrimaryKeyTag( secondtype{});
//
//    std::cout
//        <<  std::is_same<
//                secondtype,
//                sqlpp::tag::primary_key
//            >::value << "*\n";
//
//    std::cout << typeid( first_type_t< tail_type_t< sqlpp::detail::type_set<ColumnTags...>>>).name() << "&\n";
//
//    std::cout
//        <<  sqlpp::detail::is_element_of<
//                sqlpp::tag::primary_key,
//                sqlpp::detail::type_set< ColumnTags...>
//            >::value << "+\n";
//}

SQLPP_DECLARE_TABLE(
        (TestTable),
        (int_primary_key, integer, is_primary_key, require_insert, is_auto_increment)
        (int_with_default, integer, has_default(0))
        (string_with_default, text, has_default(sqlpp::null))
);

SQLPP_DECLARE_TABLE(
    (JvLayer),
    (FromLink              , integer     )
    (ChaFromNodeTp         , integer     )
    (ChaFromTileId         , integer     )
    (ChaFromIdInTile       , integer     )
    (ChaFromIsBaseLink     , integer     )
    (ChaFromDirection      , integer     )
    (ChaFromOrdinalNr      , integer     )
    (ToLink                , integer     )
    (ChaToNodeTp           , integer     )
    (ChaToTileId           , integer     )
    (ChaToIdInTile         , integer     )
    (ChaToIsBaseLink       , integer     )
    (ChaToDirection        , integer     )
    (ChaToOrdinalNr        , integer     )
    (ImageId               , integer     )
    (ImageType             , integer     )
    (Ambience              , integer     )
    (IntersectionViewId    , integer     )
    (SeqNr                 , integer     )
    (PathNr                , integer     )
    (ISIId                 , integer     )
);

int main()
{
    using namespace sqlpp;
    MockDb db;

    TestTable t;

    db(create_table( t));
}

