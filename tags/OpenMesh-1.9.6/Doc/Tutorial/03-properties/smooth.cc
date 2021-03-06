#include <iostream>
#include <vector>
// --------------------
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/Types/TriMesh_ArrayKernelT.hh>

typedef OpenMesh::TriMesh_ArrayKernelT<>  MyMesh;


int main(int argc, char **argv)
{
  MyMesh  mesh;


  // check command line options
  if (argc != 4) 
  {
    std::cerr << "Usage:  " << argv[0] << " #iterations infile outfile\n";
    return 1;
  }



  // read mesh from stdin
  if ( ! OpenMesh::IO::read_mesh(mesh, argv[2]) )
  {
     std::cerr << "Error: Cannot read mesh from " << argv[2] << std::endl;
     return 1;
  }



  // this vertex property stores the computed centers of gravity
  OpenMesh::VPropHandleT<MyMesh::Point> cogs;
  mesh.add_property(cogs);

  // smoothing mesh argv[1] times
  MyMesh::VertexIter          v_it, v_end(mesh.vertices_end());
  MyMesh::VertexVertexIter    vv_it;
  MyMesh::Point               cog;
  MyMesh::Scalar              valence;
  unsigned int                i, N(atoi(argv[1]));

  
  for (i=0; i < N; ++i)
  {
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
    {      
      mesh.property(cogs,v_it).vectorize(0.0f);
      valence = 0;
      
      for (vv_it=mesh.vv_iter( v_it ); vv_it; ++vv_it)
      {
	mesh.property(cogs,v_it) += mesh.point( vv_it );
	++valence;
      }
      mesh.property(cogs,v_it) /= valence;
    }
    
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( !mesh.is_boundary( v_it ) )
	mesh.set_point( v_it, mesh.property(cogs,v_it) );
  }


  // write mesh to stdout
  if ( ! OpenMesh::IO::write_mesh(mesh, argv[3]) )
  {
    std::cerr << "Error: cannot write mesh to " << argv[3] << std::endl;
    return 1;
  }

  return 0;
}
