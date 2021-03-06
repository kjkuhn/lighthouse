{
{-# OPTIONS -w #-}
-- The above warning supression flag is a temporary kludge.
-- While working on this module you are encouraged to remove it and fix
-- any warnings in the module. See
--     http://hackage.haskell.org/trac/ghc/wiki/Commentary/CodingStyle#Warnings
-- for details

module HaddockParse (
  parseHaddockParagraphs, 
  parseHaddockString, 
  MyEither(..)
) where

import {-# SOURCE #-} HaddockLex
import HsSyn
import RdrName
}

%tokentype { Token }

%token	'/'	{ TokSpecial '/' }
	'@'	{ TokSpecial '@' }
	'['     { TokDefStart }
	']'     { TokDefEnd }
	DQUO 	{ TokSpecial '\"' }
	URL	{ TokURL $$ }
	PIC     { TokPic $$ }
	ANAME	{ TokAName $$ }
	'/../'  { TokEmphasis $$ }
	'-'	{ TokBullet }
	'(n)'	{ TokNumber }
	'>..'	{ TokBirdTrack $$ }
	IDENT   { TokIdent $$ }
	PARA    { TokPara }
	STRING	{ TokString $$ }

%monad { MyEither String }

%name parseHaddockParagraphs  doc
%name parseHaddockString seq

%%

doc	:: { HsDoc RdrName }
	: apara PARA doc	{ docAppend $1 $3 }
	| PARA doc 		{ $2 }
	| apara			{ $1 }
	| {- empty -}		{ DocEmpty }

apara	:: { HsDoc RdrName }
	: ulpara		{ DocUnorderedList [$1] }
	| olpara		{ DocOrderedList [$1] }
        | defpara               { DocDefList [$1] }
	| para			{ $1 }

ulpara  :: { HsDoc RdrName }
	: '-' para		{ $2 }

olpara  :: { HsDoc RdrName } 
	: '(n)' para		{ $2 }

defpara :: { (HsDoc RdrName, HsDoc RdrName) }
	: '[' seq ']' seq	{ ($2, $4) }

para    :: { HsDoc RdrName }
	: seq			{ docParagraph $1 }
	| codepara		{ DocCodeBlock $1 }

codepara :: { HsDoc RdrName }
	: '>..' codepara	{ docAppend (DocString $1) $2 }
	| '>..'			{ DocString $1 }

seq	:: { HsDoc RdrName }
	: elem seq		{ docAppend $1 $2 }
	| elem			{ $1 }

elem	:: { HsDoc RdrName }
	: elem1			{ $1 }
	| '@' seq1 '@'		{ DocMonospaced $2 }

seq1	:: { HsDoc RdrName }
	: PARA seq1             { docAppend (DocString "\n") $2 }
	| elem1 seq1            { docAppend $1 $2 }
	| elem1			{ $1 }

elem1	:: { HsDoc RdrName }
	: STRING		{ DocString $1 }
	| '/../'                { DocEmphasis (DocString $1) }
	| URL			{ DocURL $1 }
	| PIC                   { DocPic $1 }
	| ANAME			{ DocAName $1 }
	| IDENT			{ DocIdentifier $1 }
	| DQUO strings DQUO	{ DocModule $2 }

strings  :: { String }
	: STRING		{ $1 }
	| STRING strings	{ $1 ++ $2 }

{
happyError :: [Token] -> MyEither String a
happyError toks = MyLeft ("parse error in doc string")

-- We don't want to make an instance for Either String,
-- since every user of the GHC API would get that instance

data MyEither a b = MyLeft a | MyRight b

instance Monad (MyEither String) where
	return          = MyRight
	MyLeft  l >>= _ = MyLeft l
	MyRight r >>= k = k r
	fail msg        = MyLeft msg
}
