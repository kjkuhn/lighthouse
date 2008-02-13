-----------------------------------------------------------------------------
-- |
-- Module      :  GHC.Exts
-- Copyright   :  (c) The University of Glasgow 2002
-- License     :  see libraries/base/LICENSE
-- 
-- Maintainer  :  cvs-ghc@haskell.org
-- Stability   :  internal
-- Portability :  non-portable (GHC Extensions)
--
-- GHC Extensions: this is the Approved Way to get at GHC-specific extensions.
--
-----------------------------------------------------------------------------

module GHC.Exts
       (
        -- * Representations of some basic types
        Int(..),Word(..),Float(..),Double(..),Integer(..),Char(..),
	Ptr(..), FunPtr(..),

        -- * Primitive operations
        module GHC.Prim,
	shiftL#, shiftRL#, iShiftL#, iShiftRA#, iShiftRL#,
        uncheckedShiftL64#, uncheckedShiftRL64#,
        uncheckedIShiftL64#, uncheckedIShiftRA64#,

	-- * Fusion
	build, augment,

	-- * Overloaded string literals
	IsString(..),

	-- * Debugging
	breakpoint, breakpointCond,

	-- * Ids with special behaviour
	lazy, inline,

       ) where

import Prelude

import GHC.Prim
import GHC.Base
import GHC.Word
import GHC.Int
import GHC.Num
import GHC.Float
import GHC.Ptr
import Data.String

