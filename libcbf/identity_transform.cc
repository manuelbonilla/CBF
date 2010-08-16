/*
    This file is part of CBF.

    CBF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    CBF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CBF.  If not, see <http://www.gnu.org/licenses/>.


    Copyright 2009, 2010 Florian Paul Schmidt
*/

#include <cbf/identity_transform.h>
#include <cbf/plugin_macros.h>

#include <cbf/debug_macros.h>

namespace CBF {
	
	#ifdef CBF_HAVE_XSD
		IdentityEffectorTransform::IdentityEffectorTransform(const IdentityEffectorTransformType &xml_instance) :
			m_Dim(xml_instance.Dimension())
		{
			m_InverseTaskJacobian = boost::numeric::ublas::identity_matrix<Float>(xml_instance.Dimension());

			 CBF_DEBUG("[IdentityEffectorTransform(const IdentityEffectorTransformType &xml_instance)]: yay!!!")
		}
		
		IdentitySensorTransform::IdentitySensorTransform(const IdentitySensorTransformType &xml_instance) :
			SensorTransform(xml_instance),
			m_Dim(xml_instance.Dimension())
		{
			CBF_DEBUG("[IdentitySensorTransform(const IdentitySensorTransformType &xml_instance)]: yay!!!")
			m_TaskJacobian = boost::numeric::ublas::identity_matrix<Float>(m_Dim,m_Dim);
		}

	#endif
	
	CBF_PLUGIN_CLASS(IdentitySensorTransform, SensorTransform)

	static XMLDerivedFactory<IdentitySensorTransform, IdentitySensorTransformType, SensorTransform, SensorTransformType> x;
} // namespace

