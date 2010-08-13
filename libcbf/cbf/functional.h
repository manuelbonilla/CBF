#ifndef CBF_FUNCTIONAL_HH
#define CBF_FUNCTIONAL_HH

#include <cbf/sensor_transform.h>

namespace CBF {
	struct SensorTransformOperation {
		SensorTransformOperation() { }

		virtual FloatVector operator()(const FloatVector &result) = 0;
		virtual FloatMatrix operator()(const FloatMatrix &result) = 0;
	};
	
	template <class VectorOperation, class MatrixOperation>
	struct GenericSensorTransformOperation : public SensorTransformOperation {
		virtual FloatVector operator()(const FloatVector &input) 
			{ return VectorOperation()(input); }

		virtual FloatMatrix operator()(const FloatMatrix &input)
			{ return MatrixOperation()(input); }
	};

#if 0
	template struct GenericSensorTransformOperation
		<std::negate<FloatVector>, std::negate<FloatMatrix> >;
#endif

	

	/**
		This class provides a way to create a SensorTransform that 
		performs the Operation each cycle on all the operands
		- The result calculation of the 
	*/
	template <class Operation>
	struct OperationSensorTransform : public SensorTransform {
		OperationSensorTransform(SensorTransformPtr operand = SensorTransformPtr()) : m_Operand(operand) { }
		SensorTransformPtr m_Operand;

		virtual void update() {
			m_Result = Operation()(m_Operand->result());
			m_TaskJacobian = Operation()(m_Operand->task_jacobian());
		}

		virtual unsigned int task_dim() const { return m_Operand->task_dim(); }
		virtual unsigned int resource_dim() const { return m_Operand->resource_dim(); }
	};

#if 0
	template struct OperationSensorTransform<
		GenericSensorTransformOperation<
			std::negate<FloatVector>,  
			std::negate<FloatMatrix>
		>
	>;
#endif

	typedef struct OperationSensorTransform<
		GenericSensorTransformOperation<
			std::negate<FloatVector>,  
			std::negate<FloatMatrix>
		>
	> NegateOperationSensorTransform;

	template <class Operation>
	struct MapSensorTransform : public SensorTransform {
		typedef std::vector<SensorTransformPtr> OperandsVector;
		OperandsVector m_Operands;

		virtual void update() {
			for (OperandsVector::iterator it = m_Operands.begin; it != m_Operands.end(); ++it) {
				m_Result = Operation((*it)->result(), (*it)->task_jacobian);
			}
		}
	};
} // namespace

#endif