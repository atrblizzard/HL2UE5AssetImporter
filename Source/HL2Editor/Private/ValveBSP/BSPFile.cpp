#include "BSPFile.hpp"
#include "IHL2Editor.h"
#include <iostream>

using namespace Valve;
using namespace BSP;

BSPFile::BSPFile( const std::string& bsp_directory, const std::string& bsp_file )
{
    parse( bsp_directory, bsp_file );
}

bool BSPFile::parse( const std::string& bsp_directory, const std::string& bsp_file )
{
    if( bsp_directory.empty() || bsp_file.empty() ) {
        return false;
    }

    std::ifstream bsp_binary( bsp_directory + bsp_file, std::ios::binary );
    if( !bsp_binary.is_open() ) {
        return false;
    }
    m_FileName = bsp_file;
    try {
        /// parse the bsp header
        bsp_binary.read( reinterpret_cast< char* >( &m_BSPHeader ), sizeof dheader_t );

        /// check bsp version/ident
        if( m_BSPHeader.m_Version < BSPVERSION ) {
            std::cout << "BSPFile::parse(): " << bsp_file << "has an unknown BSP version, trying to parse it anyway..." << std::endl;
        }
        if( m_BSPHeader.m_Ident != IDBSPHEADER ) {
            std::cout << "BSPFile::parse(): " << bsp_file << "isn't a (valid) BSP file!" << std::endl;
            return false;
        }

		/*if ( !parse_vis( bsp_binary ) ) {
			return false;
		}*/

		parse_lump_data( bsp_binary, LUMP_ENTITIES, m_Entities );

        parse_lump_data( bsp_binary, LUMP_VERTEXES, m_Vertexes );
        if( !parse_planes( bsp_binary ) ) {
            return false;
        }

        parse_lump_data( bsp_binary, LUMP_EDGES, m_Edges );
        parse_lump_data( bsp_binary, LUMP_SURFEDGES, m_Surfedges );
        parse_lump_data( bsp_binary, LUMP_LEAFS, m_Leaves );
        if( !parse_nodes( bsp_binary ) ) {
            return false;
        }

        parse_lump_data( bsp_binary, LUMP_FACES, m_Surfaces );
		parse_lump_data( bsp_binary, LUMP_ORIGINALFACES, m_OrigSurfaces );
        parse_lump_data( bsp_binary, LUMP_TEXINFO, m_Texinfos );
		parse_lump_data( bsp_binary, LUMP_TEXDATA, m_Texdatas );
		parse_lump_data( bsp_binary, LUMP_TEXDATA_STRING_TABLE, m_TexdataStringTable );
		parse_lump_data( bsp_binary, LUMP_TEXDATA_STRING_DATA, m_TexdataStringData );
        parse_lump_data( bsp_binary, LUMP_BRUSHES, m_Brushes );
        parse_lump_data( bsp_binary, LUMP_BRUSHSIDES, m_Brushsides );
		parse_lump_data( bsp_binary, LUMP_MODELS, m_Models );
        if( !parse_leaffaces( bsp_binary )
            || !parse_leafbrushes( bsp_binary )
            || !parse_polygons() ) {
            return false;
        }

		parse_lump_data(bsp_binary, LUMP_DISPINFO, m_Dispinfos);
		parse_lump_data(bsp_binary, LUMP_DISP_VERTS, m_Dispverts);
		parse_lump_data(bsp_binary, LUMP_DISP_TRIS, m_Disptris);

		parse_lump_data(bsp_binary, LUMP_CUBEMAPS, m_Cubemaps);

		if ( !parse_gamelumps( bsp_binary )
			|| !parse_staticprops( bsp_binary )
            || !parse_detailprops( bsp_binary ) ) {
			return false;
		}

    }
    catch( const std::exception& e ) {
        print_exception( "parse", e );
        return false;
    }

    return true;
}

bool BSPFile::parse_vis( std::ifstream& bsp_binary )
{
	try {
		std::vector< char > data;
		parse_lump_data( bsp_binary, LUMP_VISIBILITY, data );
		const int* start = (const int*)&data[0];

		const int numClusters = *start;
		
		for (int clusterIndex = 0; clusterIndex < numClusters; ++clusterIndex) {
			std::vector< int > visibleClusters;
			int v = start[(clusterIndex << 1) + 1];

			for (int c = 0; c < numClusters; v++) {

				if (start[v] == 0) {
					v++;
					c += 8 * start[v];
				}
				else {
					for (char bit = 1; bit != 0; bit *= 2, c++) {
						if (start[v] & bit) {
							visibleClusters.push_back(c);
						}
					}
				}

			}

			m_Visibility.push_back(visibleClusters);
		}


	}
	catch (const std::exception& e) {
		print_exception("parse_vis", e);
		return false;
	}
	return true;
}

bool BSPFile::parse_planes( std::ifstream& bsp_binary )
{
    try {
        std::vector< dplane_t > planes;
        parse_lump_data( bsp_binary, LUMP_PLANES, planes );

        m_Planes = std::vector< cplane_t >( planes.size() );

        int32_t plane_bits;
        for( size_t i = 0; i < planes.size(); ++i ) {
            auto& out = m_Planes.at( i );
            auto& in = planes.at( i );

            plane_bits = 0;
            for( size_t j = 0; j < 3; ++j ) {
                out.m_Normal( j ) = in.m_Normal( j );
                if( out.m_Normal( j ) < 0.f ) {
                    plane_bits |= 1 << static_cast< int32_t >( j );
                }
            }

            out.m_Distance = in.m_Distance;
            out.m_Type = static_cast< uint8_t >( in.m_Type );
            out.m_SignBits = static_cast< uint8_t >( plane_bits );
        }
    }
    catch( const std::exception& e ) {
        print_exception( "parse_planes", e );
        return false;
    }
    return true;
}

bool BSPFile::parse_nodes( std::ifstream& bsp_binary )
{
    try {
        std::vector< dnode_t > nodes;
        parse_lump_data( bsp_binary, LUMP_NODES, nodes );

        const auto num_nodes = nodes.size();
        m_Nodes = std::vector< snode_t >( num_nodes );

        for( size_t i = 0; i < num_nodes; ++i ) {
            auto& in = nodes.at( i );
            auto& out = m_Nodes.at( i );

            out.m_Mins = in.m_Mins;
            out.m_Maxs = in.m_Maxs;
            out.m_PlaneNum = in.m_Planenum;
            out.m_pPlane = m_Planes.data() + in.m_Planenum;
            out.m_Firstface = in.m_Firstface;
            out.m_Numfaces = in.m_Numfaces;

            for( size_t j = 0; j < 2; ++j ) {
                auto child_index = in.m_Children.at( j );
                out.m_Children.at( j ) = child_index;

                if( child_index >= 0 ) {
                    out.m_LeafChildren = nullptr;
                    out.m_NodeChildren = m_Nodes.data() + child_index;
                }
                else {
                    out.m_LeafChildren = m_Leaves.data() + ( -1 - child_index );
                    out.m_NodeChildren = nullptr;
                }
            }
        }
    }
    catch( const std::exception& e ) {
        print_exception( "parse_nodes", e );
        return false;
    }
    return true;
}

bool BSPFile::parse_leaffaces( std::ifstream& bsp_binary )
{
    try {
        parse_lump_data( bsp_binary, LUMP_LEAFFACES, m_Leaffaces );

        const auto num_leaffaces = m_Leaffaces.size();
        if( num_leaffaces > MAX_MAP_LEAFBRUSHES ) {
            std::cout << "BSPFile::parse_leaffaces(): map has to many leaffaces, parsed more than required.." << std::endl;
        }
        else if( !num_leaffaces ) {
            std::cout << "BSPFile::parse_leaffaces(): map has no leaffaces to parse!" << std::endl;
        }
    }
    catch( const std::exception& e ) {
        print_exception( "parse_leaffaces", e );
        return false;
    }
    return true;
}

bool BSPFile::parse_leafbrushes( std::ifstream& bsp_binary )
{
    try {
        parse_lump_data( bsp_binary, LUMP_LEAFBRUSHES, m_Leafbrushes );

        const auto num_leaffaces = m_Leaffaces.size();
        if( num_leaffaces > MAX_MAP_LEAFBRUSHES ) {
            std::cout << "BSPFile::parse_leaffaces(): map has to many leafbrushes, parsed more than required.." << std::endl;
        }
        else if( !num_leaffaces ) {
            std::cout << "BSPFile::parse_leaffaces(): map has no leafbrushes to parse!" << std::endl;
        }
    }
    catch( const std::exception& e ) {
        print_exception( "parse_leafbrushes", e );
        return false;
    }
    return true;
}

bool BSPFile::parse_polygons( void )
{
    try {
        m_Polygons = std::vector< Polygon >( m_Surfaces.size() );
        for( auto& surface : m_Surfaces ) {
            auto first_edge = surface.m_Firstedge;
            auto num_edges = surface.m_Numedges;

            if( num_edges < 3 || static_cast< size_t >( num_edges ) > MAX_SURFINFO_VERTS ) {
                continue;
            }
            if( surface.m_Texinfo <= 0 ) {
                continue;
            }

            Polygon polygon;
            Vector3 edge;
            for( auto i = 0; i < num_edges; ++i ) {
                auto edge_index = m_Surfedges.at( first_edge + i );
                if( edge_index >= 0 ) {
                    edge = m_Vertexes.at( m_Edges[ edge_index ].m_V.at( 0 ) ).m_Position;
                }
                else {
                    edge = m_Vertexes.at( m_Edges[ -edge_index ].m_V.at( 1 ) ).m_Position;
                }
                polygon.m_Verts.at( i ) = edge;
            }

            polygon.m_nVerts = static_cast< size_t >( num_edges );
            polygon.m_Plane.m_Origin = m_Planes.at( surface.m_Planenum ).m_Normal;
            polygon.m_Plane.m_Distance = m_Planes.at( surface.m_Planenum ).m_Distance;
            m_Polygons.push_back( polygon );
        }
    }
    catch( const std::exception& e ) {
        print_exception( "parse_polygons", e );
        return false;
    }
    return true;
}

BSP::dgamelump_t BSPFile::get_game_lump( const BSP::eGamelumpIndex gamelump_index ) const
{
	for ( auto& gamelump : m_Gamelumps ) {
		if ( gamelump.m_ID == gamelump_index ) {
			return gamelump;
		}
	}
	BSP::dgamelump_t invalid;
	invalid.m_ID = -1;
	return invalid;
}

bool BSPFile::parse_gamelumps( std::ifstream& bsp_binary )
{
	try {
		const auto& lump = m_BSPHeader.m_Lumps.at(LUMP_GAME_LUMP);
		const auto lump_size = lump.m_Filelen;
		if (!lump_size) {
			return true;
		}

		int numGameLumps;
		bsp_binary.seekg( lump.m_Fileofs );
		bsp_binary.read( reinterpret_cast<char*>(&numGameLumps), sizeof(int) );

		m_Gamelumps = std::vector< dgamelump_t >( numGameLumps );
		bsp_binary.read( reinterpret_cast< char* >( m_Gamelumps.data() ), sizeof( dgamelump_t ) * numGameLumps );
		
	}
	catch (const std::exception& e) {
		print_exception("parse_gamelumps", e);
		return false;
	}
	return true;
}

bool BSPFile::parse_staticprops( std::ifstream& bsp_binary )
{
	try {

		auto lump = get_game_lump( GAMELUMP_STATICPROPS );
		if ( lump.m_ID != GAMELUMP_STATICPROPS ) {
			return false;
		}
		const auto lump_size = lump.m_Filelen;
		if (!lump_size) {
			return true;
		}

		int numDictEntries;
		bsp_binary.seekg( lump.m_Fileofs );
		bsp_binary.read( reinterpret_cast<char*>(&numDictEntries), sizeof(int) );

		m_StaticpropStringTable = std::vector< StaticPropName_t >( numDictEntries );
		bsp_binary.read( reinterpret_cast<char*>( m_StaticpropStringTable.data() ), sizeof( StaticPropName_t ) * numDictEntries );

		int numLeafEntries;
		bsp_binary.read( reinterpret_cast<char*>(&numLeafEntries), sizeof( int ) );

		bsp_binary.seekg( sizeof( unsigned short ) * numLeafEntries, bsp_binary.cur );

		int numStaticProps;
		bsp_binary.read( reinterpret_cast<char*>(&numStaticProps), sizeof( int ) );

		switch ( lump.m_Version ) {
			case 4:
				m_Staticprops_v4 = std::vector< StaticProp_v4_t >( numStaticProps );
				bsp_binary.read( reinterpret_cast<char*>( m_Staticprops_v4.data() ), sizeof( StaticProp_v4_t ) * numStaticProps );

				break;

		case 5:
				m_Staticprops_v5 = std::vector< StaticProp_v5_t >( numStaticProps );
				bsp_binary.read( reinterpret_cast<char*>( m_Staticprops_v5.data() ), sizeof( StaticProp_v5_t ) * numStaticProps );

				break;

		case 6:
				m_Staticprops_v6 = std::vector< StaticProp_v6_t >( numStaticProps );
				bsp_binary.read( reinterpret_cast<char*>( m_Staticprops_v6.data() ), sizeof( StaticProp_v6_t ) * numStaticProps );

				break;

            case 10:

                m_Staticprops_v10 = std::vector< StaticProp_v10_t >(numStaticProps);
                bsp_binary.read(reinterpret_cast<char*>(m_Staticprops_v10.data()), sizeof(StaticProp_v10_t) * numStaticProps);

                break;
			default:
				throw std::exception("Unsupported static prop lump version");
		}
	}
	catch (const std::exception& e) {
		print_exception("parse_gamelumps", e);
		return false;
	}
	return true;
}

bool BSPFile::parse_detailprops(std::ifstream& bsp_binary)
{
    try
    {
	    const auto [m_ID, m_Flags, m_Version, m_Fileofs, m_Filelen] =
	    	get_game_lump(GAMELUMP_DETAILPROPS);
    	if (m_ID != GAMELUMP_DETAILPROPS) {
    		return false;
    	}
    	const auto lump_size = m_Filelen;
    	if (!lump_size) {
    		return true;
    	}

    	// Model dict
    	int numDictEntries;
    	bsp_binary.seekg(m_Fileofs);
    	bsp_binary.read(reinterpret_cast<char*>(&numDictEntries), sizeof(int));

    	m_DetailpropsStringTable = std::vector< DetailObjectName_t >(numDictEntries);
    	bsp_binary.read(reinterpret_cast<char*>(m_DetailpropsStringTable.data()), sizeof(DetailObjectName_t) * numDictEntries);

    	// Sprite dict
    	int numDetailSprites;
    	bsp_binary.read(reinterpret_cast<char*>(&numDetailSprites), sizeof(int));

    	m_Detailsprites = std::vector< DetailSpriteDict_t >(numDetailSprites);
    	bsp_binary.read(reinterpret_cast<char*>(m_Detailsprites.data()), sizeof(DetailSpriteDict_t) * numDetailSprites);

    	if (!IHL2Editor::Get().GetConfig()->Config.BSP.ImportDetailObjects)
    	{
    		// Detail objects
    		int numDetailObjects;
    		bsp_binary.read(reinterpret_cast<char*>(&numDetailObjects), sizeof(int));

    		m_Detailobjects = std::vector< DetailObject_t >(numDetailObjects);
    		bsp_binary.read(reinterpret_cast<char*>(m_Detailobjects.data()), sizeof(DetailObject_t) * numDetailObjects);
		}
    }
    catch (const std::exception& e) {
        print_exception("parse_gamelumps", e);
        return false;
    }
    return true;
}

void BSPFile::print_exception( const std::string& function_name, const std::exception& e ) const
{
    std::cout << "BSPFile::"
        << ( function_name.empty() ? "print_exception" : function_name ).c_str()
        << "() exception("
        << e.what()
        << ") while parsing map: "
        << m_FileName
        << std::endl;
}