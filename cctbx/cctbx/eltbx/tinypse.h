// $Id$

#ifndef CCTBX_ELTBX_TINYPSE_H
#define CCTBX_ELTBX_TINYPSE_H

#include <string>

namespace eltbx {

  namespace detail {
    struct TinyPSE_RawEntry {
      int Z;
      const char* Symbol;
      const char* Name;
      float Weight;
    };
  }

  //! Tiny Periodic System of Elements.
  /*! Reference:<br>
      CRC Handbook of Chemistry & Physics, 63rd edition, 1982-1983<br>
      CRC Handbook of Chemistry & Physics, 70th edition, 1989-1990
   */
  class TinyPSE {
    public:
      //! Lookup table entry by element symbol.
      /*! If Exact == true, the scattering factor label must exactly
          match the tabulated label. However, the lookup is not
          case-sensitive.<br>
          See also: eltbx::StripLabel()
       */
      TinyPSE(const std::string& Label, bool Exact = false);
      //! Lookup table entry by atomic number.
      TinyPSE(int Z);
      //! Return atomic number.
      inline int Z() const { return m_RawEntry->Z; }
      //! Return element symbol.
      inline const char* Symbol() const { return m_RawEntry->Symbol; }
      //! Return element name.
      inline const char* Name() const { return m_RawEntry->Name; }
      //! Return atomic weight.
      inline float Weight() const { return m_RawEntry->Weight; }
    private:
      const detail::TinyPSE_RawEntry* m_RawEntry;
  };

} // eltbx

#endif // CCTBX_ELTBX_TINYPSE_H
