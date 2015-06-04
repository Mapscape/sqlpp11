/*
 * table_spec.h
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

#ifndef SQLPP11_INCLUDE_TABLE_SPEC_H_
#define SQLPP11_INCLUDE_TABLE_SPEC_H_
#include <sqlpp11/alias_provider.h>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#define SQLPP_PREPEND_NAME( r, prefix, elem) , prefix elem

/// Generate code for a table class.
/// The generated code takes a (C++-) name and a table_name argument. The latter is the
/// name of the table in the database.
/// The code also assumes a class or namespace with name <name>_ that holds column definitions
/// for al columns of the table.
#define SQLPP_TABLE_SPEC( name, table_name, column_name_seq)\
    struct name : sqlpp::table_t< name                      \
    BOOST_PP_SEQ_FOR_EACH( SQLPP_PREPEND_NAME, BOOST_PP_CAT(name,_)::, column_name_seq)\
    >                                                       \
    {                                                       \
        SQLPP_ALIAS_STRUCT_NAMED( name, table_name);        \
    }                                                       \
    /**/

#endif /* SQLPP11_INCLUDE_TABLE_SPEC_H_ */
