Name:		GOST34.11-2012
Version:	0.13
Release:	1%{?dist}
Summary:	RFC-6986 cryptographic hash function
Group:		Productivity/Security

License:	BSD-2-Clause or GPL-2.0+
URL:		https://streebog.net
Source0:	https://github.com/adegtyarev/streebog/archive/%{version}.tar.gz

BuildRequires:	gcc
BuildRequires:	help2man
ExclusiveArch:	x86_64 i386 i586 i686 ppc ppc64 ppc64le aarch64 armv6l armv6hl armv7l armv7hl s390x

%description
Command line tool to create GOST R 34.11-2012 hash digest of files or streams.

Russian cryptographic standard GOST R 34.11-2012 establishes the hash-function
algorithm and the hash-function calculation procedure for any sequence of
binary symbols. The standard published as RFC 6986.

%package devel
Summary:	Header files for the RFC-6986 cryptographic hash function
Group:		Development/Languages/C and C++

%description devel
Header files for the GOST R 34.11-2012 hash function.

Russian cryptographic standard GOST R 34.11-2012 establishes the hash-function
algorithm and the hash-function calculation procedure for any sequence of
binary symbols. The standard published as RFC 6986.

%prep
%setup -q -n streebog-%{version}

%build
make %{?_smp_mflags} config build man

%install
mkdir -p %{buildroot}%{_bindir}
mkdir -p %{buildroot}%{_mandir}/man1
mkdir -p %{buildroot}%{_includedir}/gost3411-2012
install -pm 755 gost3411-2012 %{buildroot}%{_bindir}/
install -pm 644 gost3411-2012.1 %{buildroot}%{_mandir}/man1/
install -pm 644 gost3411-2012-const.h %{buildroot}%{_includedir}/gost3411-2012/
install -pm 644 gost3411-2012-core.h %{buildroot}%{_includedir}/gost3411-2012/
install -pm 644 gost3411-2012-mmx.h %{buildroot}%{_includedir}/gost3411-2012/
install -pm 644 gost3411-2012-precalc.h %{buildroot}%{_includedir}/gost3411-2012/
install -pm 644 gost3411-2012-ref.h %{buildroot}%{_includedir}/gost3411-2012/
install -pm 644 gost3411-2012-sse2.h %{buildroot}%{_includedir}/gost3411-2012/
install -pm 644 gost3411-2012-sse41.h %{buildroot}%{_includedir}/gost3411-2012/

%files
%defattr(-, root, root)
%{_bindir}/gost3411-2012
%{_mandir}/man1/gost3411-2012*
#
# Copy LICENSE file to %%doc for:
#
# - RedHat Enterprise Linux 6 and lower
# - CentOS 6 and lower
# - Fedora 18 and lower
#
# - Scientific Linux 6 and lower
#
# - SUSE Linux Enterprise 12 SP2 and lower
# - openSUSE Leap 42.2 and lower
#
# - Mageia 4 and lower
#
# Copy LICENSE file to %%license for:
#
# - RedHat Enterprise Linux 7 and higher
# - CentOS 7 and higher
# - Fedora 19 and higher
#
# - Scientific Linux 7 and higher
#
# - SUSE Linux Enterprise 12 SP3 and higher
# - openSUSE Leap 42.3 and higher
# - openSUSE Tumbleweed and its derivatives
#
# - Mageia 5 and higher
#
# Guideline paragraphs about using %%license macro instead of %%doc for LICENSE in spec-files:
#
# Fedora: [0] https://fedoraproject.org/wiki/Packaging:LicensingGuidelines?rd=Packaging/LicensingGuidelines#License_Text
#
# openSUSE: [1] https://en.opensuse.org/openSUSE:Specfile_guidelines#License_files
#
# Distribution detection codes: [2] https://en.opensuse.org/openSUSE:Build_Service_cross_distribution_howto
#
%if ( 0%{?sle_version} > 120200 && !0%{?is_opensuse} ) || (0%{?sle_version} > 120200 && 0%{?is_opensuse} ) || 0%{?suse_version} > 1500 || 0%{?rhel_version} > 600 || 0%{?centos_version} > 600 || 0%{?scientificlinux_version} > 600 || 0%{?fedora_version} > 18 || 0%{?mageia} > 4
%doc README.md
%license LICENSE LICENSE.GPL2
%else
%doc LICENSE LICENSE.GPL2 README.md
%endif

%files devel
%defattr(-, root, root)
%dir %{_includedir}/gost3411-2012
%{_includedir}/gost3411-2012/gost3411-2012-const.h
%{_includedir}/gost3411-2012/gost3411-2012-core.h
%{_includedir}/gost3411-2012/gost3411-2012-mmx.h
%{_includedir}/gost3411-2012/gost3411-2012-precalc.h
%{_includedir}/gost3411-2012/gost3411-2012-ref.h
%{_includedir}/gost3411-2012/gost3411-2012-sse2.h
%{_includedir}/gost3411-2012/gost3411-2012-sse41.h

%changelog
* Wed Dec 12 2018 alexey@renatasystems.org 0.13-1
- Release 0.13

* Wed Apr 25 2018 alexey@renatasystems.org 0.12-1
- Add devel package

* Mon Apr 09 2018 alexey@renatasystems.org 0.12-1
- Initial RPM release
