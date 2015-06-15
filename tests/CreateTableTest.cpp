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
#include <sqlpp11/select.h>
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




SQLPP_DECLARE_TABLE(
        (TestTable),
        (int_primary_key    , integer   , is_primary_key, require_insert, is_auto_increment)
        (int_with_default   , integer   , has_default(0))
        (string_with_default, text      , has_default(sqlpp::null))
);

SQLPP_DECLARE_TABLE(
    (TestTable2),
    (int_primary_key2   , integer     , is_primary_key)
    (int_plain_value    , integer     )
);

int main()
{
    using namespace sqlpp;
    MockDb db;

    TestTable t;
    TestTable2 p;


    // Construct a create table statement from a table definition
    db(create_table( t));

    // Construct a create table statement from the column definitions of a query.
    // This requires us to give an additional name for the table.
    auto query =
            select( t.int_primary_key, t.string_with_default, p.int_plain_value)
            .from(
                t.join( p)
                .on( t.int_with_default == p.int_primary_key2)
            )
            .where( true);
    db(create_table( query, sqlpp::alias::a));

}
