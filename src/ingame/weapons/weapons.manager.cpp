#include "weapons.manager.h"
#include "../../sounds/manager.h"
#include "../../logs/logs.h"


namespace
{
    char* get_line(const char *data)
    {
        int pos = 0;
        while (data[pos]!=0 && data[pos]!=13 && data[pos]!=10)
            ++pos;
        char *d = new char[pos+1];
        memcpy(d,data,pos);
        d[pos] = 0;
        return d;
    }
}



namespace TA3D
{

    WEAPON_MANAGER weapon_manager;

    
    WEAPON_MANAGER::WEAPON_MANAGER()
        :nb_weapons(0), weapon()
    {
        cannonshell.init();
    }


    void WEAPON_MANAGER::init()
    {
        nb_weapons = 0;
        weapon.clear();
        cannonshell.init();
    }


    WEAPON_MANAGER::~WEAPON_MANAGER()
    {
        destroy();
        weapon_hashtable.emptyHashTable();
    }

    void WEAPON_MANAGER::destroy()
    {
        cannonshell.destroy();
        weapon.clear();
        weapon_hashtable.emptyHashTable();
        weapon_hashtable.initTable(__DEFAULT_HASH_TABLE_SIZE);
        init();
    }



    int WEAPON_MANAGER::add_weapon(const String &name)
    {
        ++nb_weapons;
        weapon.resize( nb_weapons );
        weapon[nb_weapons-1].init();
        weapon[nb_weapons-1].internal_name = name;
        weapon[nb_weapons-1].nb_id = nb_weapons-1;

        weapon_hashtable.insert(String::ToLower(name), nb_weapons);

        return nb_weapons-1;
    }

    
    void WEAPON_MANAGER::load_tdf(char *data, const int size)
    {
        set_uformat(U_ASCII);
        char *pos=data;
        char *ligne=NULL;
        int nb=0;
        int index=0;
        char *limit = data + size;
        char *f;
        int n_pos=0;
        do {
            do
            {
                ++nb;
                if(ligne)
                    delete[] ligne;
                ligne = get_line(pos);
                strlwr(ligne);
                while (pos[0]!=0 && pos[0]!=13 && pos[0]!=10)
                {
                    pos++;
                    n_pos++;
                }
                while (pos[0]==13 || pos[0]==10)
                {
                    ++pos;
                    ++n_pos;
                }

                if(strstr(ligne,"//"))
                    *(strstr(ligne,"//")) = 0;
                else if( strstr(ligne,"/*") )
                    *(strstr(ligne,"/*")) = 0;
                else if( strstr(ligne,"{"))		// Skip comments
                    *(strstr(ligne,"{")) = 0;

                if(ligne[0]=='[')
                {
                    if(strstr(ligne,"]"))
                        *(strstr(ligne,"]"))=0;
                    index=add_weapon(ligne+1);
                }
                else
                    if((f=strstr(ligne,"[damage]"))) // Si on a trouvé un paragraphe référant aux dégats
                    {
                        do 	// Parcour le paragraphe
                        {
                            nb++;
                            if(ligne)
                                delete[] ligne;
                            ligne=get_line(pos);
                            strlwr(ligne);
                            while(pos[0]!=0 && pos[0]!=13 && pos[0]!=10)	pos++;
                            while(pos[0]==13 || pos[0]==10)	pos++;

                            if((f=strstr(ligne,"default=")))
                            {
                                if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                                weapon[index].damage=atoi(f+8);
                            }
                            else
                                if(( f = strstr(ligne, "=" ))) 	// Read specific damage data
                                {
                                    String unit_name = String( ligne );
                                    unit_name.trim();
                                    unit_name.resize( unit_name.find( "=" ) );
                                    int dmg = atoi( f + 1 );
                                    weapon[index].damage_hashtable.insert(unit_name, dmg);
                                }
                        } while(strstr(ligne,"}")==NULL && nb<1000 && pos<limit);
                    }
                    else if((f=strstr(ligne,"name="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].name = f+5;
                    }
                    else if((f=strstr(ligne,"id="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].weapon_id = atoi(f+3);
                    }
                    else if((f=strstr(ligne,"rendertype="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].rendertype = atoi(f+11);
                    }
                    else if((f=strstr(ligne,"ballistic="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].ballistic = (f[10]=='1');
                    }
                    else if((f=strstr(ligne,"turret="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].turret = (f[7]=='1');
                    }
                    else if((f=strstr(ligne,"noautorange="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].noautorange = (f[12]=='1');
                    }
                    else if((f=strstr(ligne,"range="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].range = atoi(f+6);
                        weapon[index].time_to_range*=weapon[index].range;
                    }
                    else if((f=strstr(ligne,"reloadtime="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].reloadtime = atof(f+11);
                    }
                    else if((f=strstr(ligne,"weaponvelocity="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].weaponvelocity = atoi(f+15)*0.5f;
                        weapon[index].time_to_range /= weapon[index].weaponvelocity;
                    }
                    else if((f=strstr(ligne,"burst="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].burst = atoi(f+6);
                    }
                    else if((f=strstr(ligne,"areaofeffect="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].areaofeffect = atoi(f+13);
                    }
                    else if((f=strstr(ligne,"startsmoke="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].startsmoke = (f[11]=='1');
                    }
                    else if((f=strstr(ligne,"endsmoke="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].endsmoke = (f[9]=='1');
                    }
                    else if((f=strstr(ligne,"firestarter="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].firestarter = atoi(f+12);
                    }
                    else if((f=strstr(ligne,"accuracy="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].accuracy = atoi(f+9);
                    }
                    else if((f=strstr(ligne,"aimrate="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].aimrate = atoi(f+8);
                    }
                    else if((f=strstr(ligne,"tolerance="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].tolerance = atoi(f+9);
                    }
                    else if((f=strstr(ligne,"holdtime="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].holdtime = atoi(f+8);
                    }
                    else if((f=strstr(ligne,"energypershot="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].energypershot = atoi(f+14);
                    }
                    else if((f=strstr(ligne,"metalpershot="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].metalpershot = atoi(f+13);
                    }
                    else if((f=strstr(ligne,"minbarrelangle="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].minbarrelangle = atoi(f+15);
                    }
                    else if((f=strstr(ligne,"unitsonly="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].unitsonly = (f[10]=='1');
                    }
                    else if((f=strstr(ligne,"edgeeffectiveness="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].edgeeffectiveness = atof(f+18);
                    }
                    else if((f=strstr(ligne,"lineofsight="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].lineofsight = (f[12]=='1');
                    }
                    else if((f=strstr(ligne,"color=")))
                    {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        int c=atoi(f+6);
                        weapon[index].color[0] = makecol(pal[c].r<<2,pal[c].g<<2,pal[c].b<<2);
                        weapon[index].color[2] = c;
                        if( weapon[index].color[2] == 232 && weapon[index].color[3] == 234 )
                        {
                            weapon[index].color[0] = makecol( pal[ 180 ].r << 2, pal[ 180 ].g << 2, pal[ 180 ].b << 2 );
                            weapon[index].color[1] = makecol( pal[ 212 ].r << 2, pal[ 212 ].g << 2, pal[ 212 ].b << 2 );
                            weapon[index].color[2] = 180;
                            weapon[index].color[3] = 212;
                        }
                    }
                    else if((f=strstr(ligne,"color2=")))
                    {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        int c=atoi(f+7);
                        weapon[index].color[1] = makecol(pal[c].r<<2,pal[c].g<<2,pal[c].b<<2);
                        weapon[index].color[3] = c;
                        if( weapon[index].color[2] == 232 && weapon[index].color[3] == 234 ) {
                            weapon[index].color[0] = makecol( pal[ 180 ].r << 2, pal[ 180 ].g << 2, pal[ 180 ].b << 2 );
                            weapon[index].color[1] = makecol( pal[ 212 ].r << 2, pal[ 212 ].g << 2, pal[ 212 ].b << 2 );
                            weapon[index].color[2] = 180;
                            weapon[index].color[3] = 212;
                        }
                    }
                    else if((f=strstr(ligne,"burstrate="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].burstrate=atof(f+10);
                    }
                    else if((f=strstr(ligne,"duration="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].duration=atof(f+9);
                    }
                    else if((f=strstr(ligne,"beamweapon="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].beamweapon=(f[11]=='1');
                    }
                    else if((f=strstr(ligne,"startvelocity="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].startvelocity=atoi(f+14)*0.5f;
                    }
                    else if((f=strstr(ligne,"weapontimer="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].weapontimer=atof(f+12);
                    }
                    else if((f=strstr(ligne,"weaponacceleration="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].weaponacceleration=atoi(f+19)*0.5f;
                    }
                    else if((f=strstr(ligne,"turnrate="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].turnrate=atoi(f+9);
                    }
                    else if((f=strstr(ligne,"model="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].model=model_manager.get_model(f+6);
                    }
                    else if((f=strstr(ligne,"smokedelay="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].smokedelay=atof(f+11);
                    }
                    else if((f=strstr(ligne,"guidance="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].guidance=(f[9]=='1');
                    }
                    else if((f=strstr(ligne,"tracks="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].tracks=(f[7]=='1');
                    }
                    else if((f=strstr(ligne,"selfprop="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].selfprop=(f[9]=='1');
                    }
                    else if((f=strstr(ligne,"waterweapon="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].waterweapon=(f[12]=='1');
                    }
                    else if((f=strstr(ligne,"smoketrail="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].smoketrail=(f[11]=='1');
                    }
                    else if((f=strstr(ligne,"flighttime="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].flighttime=atoi(f+11);
                    }
                    else if((f=strstr(ligne,"coverage="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].coverage=atoi(f+9)*0.5f;
                    }
                    else if((f=strstr(ligne,"vlaunch="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].vlaunch=(f[8]=='1');
                    }
                    else if((f=strstr(ligne,"paralyzer="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].paralyzer=(f[10]=='1');
                    }
                    else if((f=strstr(ligne,"stockpile="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].stockpile=(f[10]=='1');
                    }
                    else if((f=strstr(ligne,"targetable="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].targetable=(f[11]=='1');
                    }
                    else if((f=strstr(ligne,"interceptor="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].interceptor=(f[12]=='1');
                    }
                    else if((f=strstr(ligne,"commandfire="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].commandfire=(f[12]=='1');
                    }
                    else if((f=strstr(ligne,"cruise="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].cruise=(f[7]=='1');
                    }
                    else if((f=strstr(ligne,"propeller="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].propeller=(f[10]=='1');
                    }
                    else if((f=strstr(ligne,"twophase="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].twophase=(f[9]=='1');
                    }
                    else if((f=strstr(ligne,"dropped="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].dropped=(f[8]=='1');
                    }
                    else if((f=strstr(ligne,"burnblow="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].burnblow=(f[9]=='1');
                    }
                    else if((f=strstr(ligne,"toairweapon="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].toairweapon=(f[12]=='1');
                    }
                    else if((f=strstr(ligne,"noexplode="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].noexplode=(f[10]=='1');
                    }
                    else if((f=strstr(ligne,"shakemagnitude="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].shakemagnitude=atoi(f+15);
                    }
                    else if((f=strstr(ligne,"metal="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].metal=atoi(f+6);
                    }
                    else if((f=strstr(ligne,"energy="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].energy=atoi(f+7);
                    }
                    else if((f=strstr(ligne,"shakeduration="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].shakeduration=atof(f+14);
                    }
                    else if((f=strstr(ligne,"waterexplosiongaf="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].waterexplosiongaf = f+18;
                    }
                    else if((f=strstr(ligne,"waterexplosionart="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].waterexplosionart = f+18;
                    }
                    else if((f=strstr(ligne,"lavaexplosiongaf="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].lavaexplosiongaf = f+17;
                    }
                    else if((f=strstr(ligne,"lavaexplosionart="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].lavaexplosionart = f+17;
                    }
                    else if((f=strstr(ligne,"explosiongaf="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].explosiongaf = f+13;
                    }
                    else if((f=strstr(ligne,"explosionart="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].explosionart = f+13;
                    }
                    else if((f=strstr(ligne,"soundtrigger="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].soundtrigger = f+13;
                        sound_manager->loadSound( weapon[index].soundtrigger , true );
                    }
                    else if((f=strstr(ligne,"soundhit="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].soundhit = f+9;
                        sound_manager->loadSound( weapon[index].soundhit , true );
                    }
                    else if((f=strstr(ligne,"soundstart="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].soundstart = f+11;
                        sound_manager->loadSound( weapon[index].soundstart , true );
                    }
                    else if((f=strstr(ligne,"soundwater="))) {
                        if((strstr(ligne,";")))	*(strstr(ligne,";"))=0;
                        weapon[index].soundwater = f+11;
                        sound_manager->loadSound(weapon[index].soundwater , true);
                    }
                    else {
                        if(strlen(ligne)>1 && strstr(ligne,"{")==NULL && strstr(ligne,"}")==NULL)
                            LOG_WARNING("Unknown weapon: " << ligne);
                    }

            } while(strstr(ligne,"}")==NULL && pos<limit);

            delete[] ligne;
            ligne = NULL;

        } while (pos<limit);
    }



} // namespace TA3D
