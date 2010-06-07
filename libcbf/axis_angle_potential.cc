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

#include <cbf/axis_angle_potential.h>
#include <cbf/plugin_impl_macros.h>
#include <cbf/quaternion.h>

namespace CBF {

		Float AxisAnglePotential::distance(const FloatVector &v1, const FloatVector &v2) {

			Quaternion q1; q1.from_axis_angle3(v1);
			Quaternion q2; q2.from_axis_angle3(v2);

			Quaternion q3 = q1.conjugate() * q2;

			//q3.axis_angle();
			Float angle = acos(q3.w) * 2.0;
			if (angle > M_PI) angle -= 2.0 * M_PI;
			angle = fabs(angle);
			CBF_DEBUG("angle [distance]: " << angle)

			return angle;
		}


		void AxisAnglePotential::gradient (
			FloatVector &result, 
			const std::vector<FloatVector > &references, 
			const FloatVector &input
		) {
			CBF_DEBUG("[AxisAnglePotential]: input: " << input)
			CBF_DEBUG("[AxisAnglePotential]: ref: " << references[0])
			Quaternion in;
			in.from_axis_angle3(input);
			CBF_DEBUG("q_in: " << in)

			Quaternion ref;
			ref.from_axis_angle3(references[0]);
			CBF_DEBUG("q_ref: " << ref)

			Quaternion step = qslerp(in, ref, m_Coefficient);
			CBF_DEBUG("step: " << step)

			Quaternion res = step  * in.conjugate();
			CBF_DEBUG("res: " << res)

			result.resize(3);
			res.to_axis_angle3(result);
			CBF_DEBUG("result: " << result)

			if(norm(result) > m_MaxGradientStepNorm)
				result *= m_MaxGradientStepNorm/norm(result);

			//TODO: fix when using all references
			m_Converged = check_convergence(distance(input, references[0]), norm(result));
			CBF_DEBUG("Result: " << result)
		}





	#ifdef CBF_HAVE_XSD
		AxisAnglePotential::AxisAnglePotential(const AxisAnglePotentialType &xml_instance) :
			Potential(xml_instance) {
			CBF_DEBUG("[AxisAnglePotential(const AxisAnglePotentialType &xml_instance)]: yay!")
			CBF_DEBUG("Coefficient: " << xml_instance.Coefficient())
			m_Coefficient = xml_instance.Coefficient();
		}
	#endif
	
	CBF_PLUGIN_CLASS(AxisAnglePotential, Potential)
} // namespace