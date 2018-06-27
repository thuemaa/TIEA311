#include "MatrixStack.h"

MatrixStack::MatrixStack()
{
	// Initialize the matrix stack with the identity matrix.
	std::vector< Matrix4f > m_matrices = { Matrix4f::identity() };
	//matriisiStack.push_back(Matrix4f::identity());
}

void MatrixStack::clear()
{
	// Revert to just containing the identity matrix.
	//MatrixStack *this = MatrixStack();
	//while (m_matrices.size() > 1) {
	//	m_matrices.pop_back();
	//}

	m_matrices.clear();
	m_matrices.push_back(Matrix4f::identity());

} 

Matrix4f MatrixStack::top()
{
	// Return the top of the stack
	return Matrix4f(m_matrices.back());
}

void MatrixStack::push( const Matrix4f& m )
{	
	// Push m onto the stack.
	// Your stack should have OpenGL semantics:
	// the new top should be the old top multiplied by m

	m_matrices.push_back(m_matrices.back() * m);
}

void MatrixStack::pop()
{
	// Remove the top element from the stack
	m_matrices.pop_back();
}
