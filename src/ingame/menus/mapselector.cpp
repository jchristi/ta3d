
#include "mapselector.h"
#include "../../misc/paths.h"
#include <algorithm>
#include "../../tnt.h"
#include "../../logs/logs.h"
#include "../../languages/i18n.h"




namespace TA3D
{
namespace Menus
{

    namespace
    {
        //! \brief Predicate to sort a vector of string
        bool sortForListOfMaps(const String& u, const String& v)
        {
            return u < v;
        }
    }

    void MapSelector::SortListOfMaps(ListOfMaps& out)
    {
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "sorting list of maps");
        std::sort(out.begin(), out.end(), sortForListOfMaps);
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "done");
    }






    bool MapSelector::Execute(const String& preSelectedMap, String& mapName)
    {
        MapSelector m(preSelectedMap);
        bool r = m.execute();
        mapName = (r) ? mapName = m.selected() : "";
        return (r && (!mapName.empty()));
    }





    MapSelector::MapSelector()
        :Abstract(),
        pSelectedMap(), pDefaultSelectedMap(""), pCachedSizeOfListOfMaps(0),
        pMiniMapTexture(0),
        pLastMapIndex(-1), pMiniMapObj(NULL), dx(0), dy(0),
        pMiniMapX1(0.0f), pMiniMapY1(0.0f), pMiniMapX2(0.0f), pMiniMapY2(0.0f)
    {}


    MapSelector::MapSelector(const String& preSelectedMap)
        :Abstract(),
        pSelectedMap(), pDefaultSelectedMap(preSelectedMap), pCachedSizeOfListOfMaps(0),
        pMiniMapTexture(0),
        pLastMapIndex(-1), pMiniMapObj(NULL), dx(0), dy(0),
        pMiniMapX1(0.0f), pMiniMapY1(0.0f), pMiniMapX2(0.0f), pMiniMapY2(0.0f)
    {}


    MapSelector::~MapSelector()
    {
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "reseting mini map texture");
        ResetTexture(pMiniMapTexture);
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "done");
    }


    bool MapSelector::doInitialize()
    {
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "Entering...");

        loadAreaFromTDF("map setup", "gui/mapsetup.area");
        // Load all maps, if any
        if (!preloadAllAvailableMaps()) // should abort if no map is present
            return false;

        // Texture for the mini map
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "reseting mini map texture");
        ResetTexture(pMiniMapTexture);
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "done");

        pSelectedMap = pDefaultSelectedMap;

        pLastMapIndex = -1;
        dx = 0;
        dy = 0;
        pMiniMapX1 = 0.0f;
        pMiniMapY1 = 0.0f;
        pMiniMapX2 = 0.0f;
        pMiniMapY2 = 0.0f;
        pMiniMapMiddleX = 0.0f;
        pMiniMapMiddleY = 0.0f;

        // The mini map object
        pMiniMapObj = pArea->get_object("mapsetup.minimap");
        if (!pMiniMapObj)
            LOG_ERROR(LOG_PREFIX_MENU_MAPSELECTOR << "Can not find the GUI object `mapsetup.minimap`");
        else
        {
            pMiniMapObj->Data = 0;
            // Save previous values for the coordinate of the mini map
            pMiniMapX1 = pMiniMapObj->x1;
            pMiniMapY1 = pMiniMapObj->y1;
            pMiniMapX2 = pMiniMapObj->x2;
            pMiniMapY2 = pMiniMapObj->y2;
            // Scaled size
            pMiniMapMiddleX = (pMiniMapX1 + pMiniMapX2) * 0.5f;
            pMiniMapMiddleY = (pMiniMapY1 + pMiniMapY2) * 0.5f;
            // Re positionate the object
            scaleAndRePosTheMiniMap();
        }

        // The control which contains all available maps
        pMapListObj = pArea->get_object("mapsetup.map_list");
        reloadMapsForGUIControl();
        // Select the default map
        autoSelectMap(pDefaultSelectedMap);

        return true;
    }

    void MapSelector::reloadMapsForGUIControl()
    {
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "reloadMapsForGUIControl()");
        if (pMapListObj)
        {
            // Load all maps
            pMapListObj->Text.resize(pCachedSizeOfListOfMaps);
            int indx(0);
            for (ListOfMaps::const_iterator i = pListOfMaps.begin(); i != pListOfMaps.end(); ++i, ++indx)
                pMapListObj->Text[indx] = *i;
        }
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "done");
    }


    void MapSelector::autoSelectMap(const String& shortName)
    {
        if (!pMapListObj)
            return;
        const String::size_type l = shortName.length();
        if (l < 9)
            return;
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "autoSelectMap()");
        const String s(shortName.substr(5, l - 9));
        int indx(0);
        for (ListOfMaps::const_iterator i = pListOfMaps.begin(); i != pListOfMaps.end(); ++i, ++indx)
        {
            if (s == *i)
            {
                pMapListObj->Pos = indx;
                pMapListObj->Data = indx;
                doGoSelectSingleMap(indx);
                return;
            }
        }
        // Default value
        pMapListObj->Pos = 0;
        pMapListObj->Data = 0;
        doGoSelectSingleMap(0);
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "done");
    }


    void MapSelector::scaleAndRePosTheMiniMap(const float coef /* = 504.0f */)
    {
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "scaleAndRePosTheMiniMap()");
        LOG_ASSERT(coef != 0.0f); // Division by zero

        if (pMiniMapObj)
        {
            float ldx = dx * (pMiniMapX2 - pMiniMapX1) / coef;
            float ldy = dy * (pMiniMapY2 - pMiniMapY1) / coef;
            pMiniMapObj->x1 = pMiniMapMiddleX - ldx;
            pMiniMapObj->y1 = pMiniMapMiddleY - ldy;
            pMiniMapObj->x2 = pMiniMapMiddleX + ldx;
            pMiniMapObj->y2 = pMiniMapMiddleY + ldy;
            pMiniMapObj->u2 = dx / 252.0f;
            pMiniMapObj->v2 = dy / 252.0f;
        }
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "done");
    }

    void MapSelector::doFinalize()
    {
        if (!pSelectedMap.empty())
            LOG_INFO(LOG_PREFIX_MENU_MAPSELECTOR << "The map `" << pSelectedMap << "` has been selected.");
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "Done.");
    }


    void MapSelector::waitForEvent()
    {
        bool keyIsPressed(false);
        do
        {
            // Get if a key was pressed
            keyIsPressed = keypressed();
            // Grab user events
            pArea->check();
            // Wait to reduce CPU consumption
            rest(TA3D_MENUS_RECOMMENDED_TIME_MS_FOR_RESTING);

        } while (pMouseX == mouse_x && pMouseY == mouse_y && pMouseZ == mouse_z && pMouseB == mouse_b
                 && mouse_b == 0
                 && !key[KEY_ENTER] && !key[KEY_ESC] && !key[KEY_SPACE] && !key[KEY_C]
                 && !keyIsPressed && !pArea->scrolling);
    }


    bool MapSelector::maySwitchToAnotherMenu()
    {
        // Aborting
        if (pArea->get_state("mapsetup.b_cancel") || key[KEY_ESC])
        {
            pSelectedMap = pDefaultSelectedMap;
            return true;
        }

        // Go ! Select the map !
        if (pArea->get_state("mapsetup.b_ok") || key[KEY_ENTER])
            return true;

        // Selection change
        if (pMapListObj)
            return doGoSelectSingleMap(pMapListObj->Pos);

        return false;
    }


    bool MapSelector::doGoSelectSingleMap(const int mapIndex)
    {
        // Bounds checking
        if (pLastMapIndex == mapIndex || mapIndex < 0 || mapIndex >= pCachedSizeOfListOfMaps)
            return false;
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "doGoSelectSingleMap()");

        // Cached value
        pLastMapIndex = mapIndex;

        // Logs
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "`" << pListOfMaps[mapIndex]
                  << "` has been selected (indx: " << mapIndex << ")");

        // The new map name
        pSelectedMap = "maps";
        pSelectedMap += '\\'; // Paths::Separator;
        pSelectedMap += pListOfMaps[mapIndex];
        pSelectedMap += ".tnt";
        // Reload the mini map
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "loading mini map texture");
        ResetTexture(pMiniMapTexture, load_tnt_minimap_fast(pSelectedMap, dx, dy));
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "mini map loaded");

        // OTA
        String otaMap("maps");
        otaMap += '\\'; // Paths::Separator;
        otaMap += pListOfMaps[mapIndex];
        otaMap += ".ota";
        uint32 otaSize(0);
        MAP_OTA mapOTA;
        if (byte* data = HPIManager->PullFromHPI(otaMap, &otaSize))
        {
            LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "loading OTA data");
            mapOTA.load((char*)data, otaSize);
            delete[] data;
            LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "ota data loaded");
        }

        // Update the mini map
        doUpdateMiniMap();
        // Map Info
        doResetAreaCaptionFromMapOTA(mapOTA);

        return false;
    }


    void MapSelector::doUpdateMiniMap()
    {
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "doUpdateMiniMap()");
        if (pMiniMapObj)
        {
            // Swap textures
            ResetTexture(pMiniMapObj->Data, pMiniMapTexture);
            pMiniMapTexture = 0;

            // Resizing
            scaleAndRePosTheMiniMap();
        }
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "done");
    }

    void MapSelector::doResetAreaCaptionFromMapOTA(MAP_OTA& mapOTA)
    {
        String title;

        // Name of the mission
        if(!mapOTA.missionname.empty())
            title << mapOTA.missionname << "\n";
        // Maximum allowed players for this map
        if(!mapOTA.numplayers.empty())
            title << "\n" << I18N::Translate("players: ") << mapOTA.numplayers << "\n";
        // Description
        if(!mapOTA.missiondescription.empty())
            title << "\n" << mapOTA.missiondescription;

        // Change the caption
        pArea->set_caption("mapsetup.map_info", title);
    }


    bool MapSelector::MapIsForNetworkGame(const String& mapShortName)
    {
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "MapIsForNetworkGame(" << mapShortName << ")");
        uint32 ota_size=0;
        byte* data = HPIManager->PullFromHPI(String("maps\\") + mapShortName + String(".ota"), &ota_size);
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "ota data extracted");
        if(data)
        {
            MAP_OTA	map_data;	// Using MAP_OTA because it's faster than cTAFileParser that fills a hash_table object
            map_data.load((char*)data, ota_size);
            bool isNetworkGame = map_data.network;
            delete[] data;
            map_data.destroy();
            LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "done (true)");
            return isNetworkGame;
        }
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "done (false)");
        return false;
    }


    void MapSelector::GetMultiPlayerMapList(ListOfMaps& out)
    {
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "GetMultiPlayerMapList()");
        // Clear the map
        out.clear();

        // Load all available maps, without any distinction
        ListOfMaps allMaps;
        if (HPIManager->getFilelist("maps\\*.tnt", allMaps) > 0)
        {
            LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "checking found maps");
            for (ListOfMaps::const_iterator it = allMaps.begin(); it != allMaps.end(); ++it)
            {
                const String::size_type l(it->length());
                if (l < 9)
                    continue;
                LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "checking " << *it);
                const String newMapName(it->substr(5, l - 9));
                if (MapSelector::MapIsForNetworkGame(newMapName))
                {
                    LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << " Found `" << newMapName << "`");
                    out.push_back(newMapName);
                }
            }
            SortListOfMaps(out);
        }
        LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << "end of GetMultiPlayerMapList");
    }


    bool MapSelector::preloadAllAvailableMaps()
    {
        GetMultiPlayerMapList(pListOfMaps);
        pCachedSizeOfListOfMaps = pListOfMaps.size();
        switch (pCachedSizeOfListOfMaps)
        {
            case 0:
                {
                    Popup(I18N::Translate("Error"), I18N::Translate("No map found"));
                    LOG_ERROR(LOG_PREFIX_MENU_MAPSELECTOR << "No maps have been found.");
                    return false;
                }
            case 1: LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << pCachedSizeOfListOfMaps
                             << " map only has been found"); break;
            default: LOG_DEBUG(LOG_PREFIX_MENU_MAPSELECTOR << pCachedSizeOfListOfMaps
                              << " maps have been found");
        }
        return true;
    }


} // namespace Menus
} // namespace TA3D


