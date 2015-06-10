/*
 * declare_table.h
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

#ifndef SQLPP11_DECLARE_TABLE_H_
#define SQLPP11_DECLARE_TABLE_H_

#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/cat.hpp>

#include <sqlpp11/table_spec.h>
#include <sqlpp11/column_spec.h>
#include <sqlpp11/type_traits.h>

/**
 * Declare a table and its metadata.
 * This macro can be invoked in the following way:
 * \code
 * SQLPP_DECLARE_TABLE(
 *     ( <table_name>, [table options]...),
 *     (column_name, column_type, [column_options]...)
 *     ...
 *     )
 * \endcode
 */
#define SQLPP_DECLARE_TABLE( table_tuple, column_semi_seq)  \
    SQLPP_DECLARE_TABLE_IMPL(                               \
        BOOST_PP_TUPLE_ELEM( 0, table_tuple),               \
        BOOST_PP_TUPLE_ELEM( 0, table_tuple),               \
        BOOST_PP_TUPLE_TO_SEQ( table_tuple),                \
        SQLPP_WRAP_SEQUENCE( column_semi_seq)               \
        )                                                   \
        /**/

/**
 * This is a variant of SQLPP_DECLARE_TABLE that allows users to
 * specify a C++ class name and a different SQL table name.
 */
#define SQLPP_DECLARE_TABLE_ALT( cpp_name, table_tuple, column_semi_seq)  \
    SQLPP_DECLARE_TABLE_IMPL(                               \
        cpp_name                                            \
        BOOST_PP_TUPLE_ELEM( 0, table_tuple)                \
        BOOST_PP_TUPLE_TO_SEQ( table_tuple),                \
        SQLPP_WRAP_SEQUENCE( column_semi_seq)               \
        )                                                   \
        /**/

// helper macros for SQLPP_WRAP_SEQUENCE
#define SQLPP_WRAP_SEQUENCE_0(...)            \
    ((__VA_ARGS__)) SQLPP_WRAP_SEQUENCE_1
#define SQLPP_WRAP_SEQUENCE_1(...)            \
    ((__VA_ARGS__)) SQLPP_WRAP_SEQUENCE_0
#define SQLPP_WRAP_SEQUENCE_0_END
#define SQLPP_WRAP_SEQUENCE_1_END

// turn a semi-sequence, e.g.  (a,b)(c)(d,e,f)(g,h) into a true
// sequence of pp tuples, e.g. ((a,b))((c))((d,e,f))((g,h))
#define SQLPP_WRAP_SEQUENCE( semi_seq)                  \
    BOOST_PP_CAT( SQLPP_WRAP_SEQUENCE_0 semi_seq, _END) \
    /**/

// tuple_elem to be used in FOR_EACH macro.
// for some reason, using this inside a TRANSFORM breaks the preprocessor
#define SQLPP_TUPLE_ELEM( dummy1_, index, tuple)\
    (BOOST_PP_TUPLE_ELEM( index, tuple))

#define COLUMN_SPEC_TO_SEQ( ...)\
    BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__, is_column))

#define SQLPP_COLUMN_DEF_TO_SPEC( dummy1_, dummy2_, tuple)\
    SQLPP_COLUMN_SPEC_FROM_SEQ(COLUMN_SPEC_TO_SEQ tuple);

// extract the first element from each tuple for a given
// sequence of tuples.
#define SQLPP_COLUMN_NAMES( column_seq)     \
    BOOST_PP_SEQ_FOR_EACH( SQLPP_TUPLE_ELEM, 0, column_seq)

#define SQLPP_COLUMN_DEF_SPEC( name, column_seq_seq)    \
    struct BOOST_PP_CAT( name, _)                       \
    {                                                   \
    column_seq_seq                                      \
    }                                                   \

#define SQLPP_DECLARE_TABLE_IMPL( cpp_name, sql_name, table_def_seq, column_seq)    \
    SQLPP_COLUMN_DEF_SPEC(                                      \
        cpp_name,                                               \
        BOOST_PP_SEQ_FOR_EACH( SQLPP_COLUMN_DEF_TO_SPEC, _, column_seq)\
    );                                                          \
    SQLPP_TABLE_SPEC(                                           \
        cpp_name,                                               \
        sql_name,                                               \
        SQLPP_COLUMN_NAMES( column_seq)                         \
        )                                                       \
    /**/

#endif /* SQLPP11_DECLARE_TABLE_H_ */
