#ifndef CBF_FUNCTIONAL_HH
#define CBF_FUNCTIONAL_HH

#include <cbf/sensor_transform.h>
#include <cbf/debug_macros.h>

#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

namespace CBF {
	namespace ublas = boost::numeric::ublas;
	/** 
		A base type for defining Operations on SensorTransforms
	*/
	struct UnarySensorTransformOperation {
		UnarySensorTransformOperation() { }

		virtual FloatVector operator()(const FloatVector &result) = 0;
		virtual FloatMatrix operator()(const FloatMatrix &result) = 0;
	};

	/**
		A derived type that can be used to produce new UnarySensorTransformOperations
		from template functors..

		The only requirement for the two functors is that they provide
		FloatVector operator()(const FloatVector &input)

		and 

		FloatMatrix operator()(const FloatMatrix &input)
	*/
	template <class VectorOperation, class MatrixOperation>
	struct GenericSensorTransformOperation : public UnarySensorTransformOperation {
		virtual FloatVector operator()(const FloatVector &input) 
			{ return VectorOperation()(input); }

		virtual FloatMatrix operator()(const FloatMatrix &input)
			{ return MatrixOperation()(input); }
	};


	/**
		If the task space of the sensor transform is built from same sized blocks
		this this class allows to apply a function to the individual blocks
	*/
	template <class VectorOperation, class MatrixOperation>
	struct MapGenericBlockWiseSensorTransformOperation : public UnarySensorTransformOperation {
		unsigned int blocksize;

		MapGenericBlockWiseSensorTransformOperation(unsigned int blocksize = 0) : 
			blocksize(blocksize) { }

		virtual FloatVector operator()(const FloatVector &input) 
		{
			CBF_DEBUG("do some stuff")
			FloatVector tmp(input.size());
			for (unsigned int i = 0, rows = input.size(); i < rows; i += blocksize) {
				CBF_DEBUG(i << " " << blocksize)
				ublas::vector_range<FloatVector> vr(tmp, ublas::range(i, i+blocksize));
				ublas::vector_range<const FloatVector> vir(input, ublas::range(i, i+blocksize));
				vr.assign(VectorOperation()(vir));
			}
			return tmp;
		}

		virtual FloatMatrix operator()(const FloatMatrix &input)
		{
			return MatrixOperation()(input); 
			FloatMatrix tmp(input.size1(), input.size2());
			for (unsigned int i = 0, rows = input.size1();  i < rows; i += blocksize) {
				CBF_DEBUG(i << " " << blocksize)
				ublas::matrix_range<FloatMatrix> vr(tmp, ublas::range(i, i+blocksize), ublas::range(0, tmp.size2()));
				ublas::matrix_range<const FloatMatrix> vir(input, ublas::range(i, i+blocksize),  ublas::range(0, tmp.size2()));
				vr.assign(MatrixOperation()(vir));
			}
			return tmp;
		}
	};



	/**
		This class provides a way to create a SensorTransform that 
		performs the Operation each cycle on the operand
	*/
	#ifdef CBF_HAVE_XSD
		template <class Operation, class XMLType>
	#else
		template <class Operation>
	#endif
	struct OperationSensorTransform : public SensorTransform {
		SensorTransformPtr m_Operand;
		Operation m_Operation;

		OperationSensorTransform(Operation &operation, SensorTransformPtr operand = SensorTransformPtr()) 
			: m_Operand(operand), m_Operation(operation) { }

		OperationSensorTransform(const XMLType &xml_instance) : SensorTransform(xml_instance) { }

		virtual void set_resource(ResourcePtr res) {
			m_Operand->set_resource(res);
		}

		virtual void update() {
			m_Operand->update();
			m_Result = m_Operation(m_Operand->result());
			m_TaskJacobian = m_Operation(m_Operand->task_jacobian());
		}

		virtual unsigned int task_dim() const { return m_Operand->task_dim(); }
		virtual unsigned int resource_dim() const { return m_Operand->resource_dim(); }
	};

	typedef struct OperationSensorTransform<
		GenericSensorTransformOperation<
			std::negate<FloatVector>,  
			std::negate<FloatMatrix>
		>,
		NegateOperationSensorTransformType
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

