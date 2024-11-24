#include "bz_img.h"

bz_uint bz_img_cvrt(bz_u8* d , bz_img_fmt dformat , bz_u8* s , bz_img_fmt sformat , bz_uint scount)
{
    //参数检查
    if(NULL == s || NULL == d || 0 == scount)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return 0;
    }
    if(dformat == sformat)
    {
        bz_errno = BZ_FMT_THE_SAME_CVRT_ERROR;
        return 0;
    }

    //YU12转RGB888
    if(RGB888 == dformat && YUV420P_PLANNAR_YU12 == sformat)
    {
        if(0 != scount % 6)
        {
            bz_errno = BZ_FMT_SCOUNT_ERROR;
            return 0;
        }

        //源数据中Y、U、V分量各自的首地址
        bz_u8* sy = s;
        bz_u8* su = s + scount * 4 / 6;
        bz_u8* sv = s + scount * 5 / 6;
        //源数据Y分量索引
        bz_uint index = 0;

        //开始转换
        bz_int r , g , b;
        bz_uint temp = scount * 4 / 6;
        while(index < temp)
        {
            //r = sy[index]                                  + 1.4075 * (sv[index / 4] - 128);
            //g = sy[index] - 0.3455 * (su[index / 4] - 128) - 0.7169 * (sv[index / 4] - 128);
            //b = sy[index] + 1.7790 * (su[index / 4] - 128);
            r = (298 * (sy[index] - 16)                               + 409 * (sv[index / 4] - 128) + 128) >> 8;
            g = (298 * (sy[index] - 16) - 100 * (su[index / 4] - 128) - 208 * (sv[index / 4] - 128) + 128) >> 8;
            b = (298 * (sy[index] - 16) + 516 * (su[index / 4] - 128)                               + 128) >> 8;

            r = r < 0 ? 0 : (r > 255 ? 255 : r);
            g = g < 0 ? 0 : (g > 255 ? 255 : g);
            b = b < 0 ? 0 : (b > 255 ? 255 : b);

            d[index * 3] = r;
            d[index * 3 + 1] = g;
            d[index * 3 + 2] = b;

            index++;
        }
        return index * 3;
    }
    //RGB888转YU12
    if(YUV420P_PLANNAR_YU12 == dformat && RGB888 == sformat)
    {
        if(0 != scount % 12)
        {
            bz_errno = BZ_FMT_SCOUNT_ERROR;
            return 0;
        }

        //目的数据中Y、U、V分量应该处于的首地址
        bz_u8* dy = d;
        bz_u8* du = d + scount / 3;
        bz_u8* dv = d + scount / 3 + scount / 3 / 4;
        //目的数据Y分量索引
        bz_uint index = 0;

        //开始转换
        bz_int y , u , v;
        bz_uint temp = scount / 3;
        while(index < temp)
        {
            //y = 0.298 * s[3 * index] + 0.612 * s[3 * index + 1] + 0.117 * s[3 * index + 2];
            //y = y < 16 ? 16 : (y > 235 ? 235 : y);
            y = ((66 * s[3 * index] + 129 * s[3 * index + 1] + 25 * s[3 * index + 2] + 128) >> 8) + 16;
            dy[index] = y;

            if(0 == index % 4)
            {
                //u = -0.168 * s[3 * index] - 0.330 * s[3 * index + 1] + 0.498 * s[3 * index + 2] + 128;
                //u = u < 16 ? 16 : (u > 240 ? 240 : u);
                u = ((-38 * s[3 * index] - 74 * s[3 * index + 1] + 112 * s[3 * index + 2] + 128) >> 8) + 128;
                du[index / 4] = u;

                //v = 0.449 * s[3 * index] - 0.435 * s[3 * index + 1] - 0.083 * s[3 * index + 2] + 128;
                //v = v < 16 ? 16 : (v > 240 ? 240 : v);
                v = ((112 * s[3 * index] - 94 * s[3 * index + 1] - 18 * s[3 * index + 2] + 128) >> 8) + 128;
                dv[index / 4] = v;
            }

            index++;
        }
        return index * 3 / 2;
    }

    //HSV_DBL转RGB888
    if(RGB888 == dformat && HSV_DBL == sformat)
    {
        if(0 != scount % 3)
        {
            bz_errno = BZ_FMT_SCOUNT_ERROR;
            return 0;
        }

        bz_dbl* s_dbl = (bz_dbl*)s;

        bz_dbl hi , f , p , q , t;
        for(bz_uint i = 0 ; i < scount ; i += 3)
        {
            hi = (bz_int)(s_dbl[i] / 60);
            f = s_dbl[i] / 60 - hi;
            p = s_dbl[i + 2] * (1 - s_dbl[i + 1]);
            q = s_dbl[i + 2] * (1 - f * s_dbl[i + 1]);
            t = s_dbl[i + 2] * (1 - (1 - f) * s_dbl[i + 1]);

            switch((bz_int)hi)
            {
            case 0:
                d[i] = s_dbl[i + 2] * 255;
                d[i + 1] = t * 255;
                d[i + 2] = p * 255;
                break;
            case 1:
                d[i] = q * 255;
                d[i + 1] = s_dbl[i + 2] * 255;
                d[i + 2] = p * 255;
                break;
            case 2:
                d[i] = p * 255;
                d[i + 1] = s_dbl[i + 2] * 255;
                d[i + 2] = t * 255;
                break;
            case 3:
                d[i] = p * 255;
                d[i + 1] = q * 255;
                d[i + 2] = s_dbl[i + 2] * 255;
                break;
            case 4:
                d[i] = t * 255;
                d[i + 1] = p * 255;
                d[i + 2] = s_dbl[i + 2] * 255;
                break;
            default:
                d[i] = s_dbl[i + 2] * 255;
                d[i + 1] = p * 255;
                d[i + 2] = q * 255;
                break;
            }
        }

        return scount;
    }
    //RGB888转HSV_DBL
    if(HSV_DBL == dformat && RGB888 == sformat)
    {
        if(0 != scount % 3)
        {
            bz_errno = BZ_FMT_SCOUNT_ERROR;
            return 0;
        }

        bz_dbl* d_dbl = (bz_dbl*)d;

        bz_dbl r , g , b; 
        bz_dbl max , min , dif;
        for(bz_uint i = 0 ; i < scount ; i += 3)
        {
            r = (bz_dbl)(s[i]) / 255;
            g = (bz_dbl)(s[i + 1]) / 255;
            b = (bz_dbl)(s[i + 2]) / 255;

            max = r;
            max = g > max ? g : max;
            max = b > max ? b : max;
            min = r;
            min = g < min ? g : min;
            min = b < min ? b : min;
            dif = max - min;

            if(max == r)
                d_dbl[i] = 60 * (g - b) / dif;
            else if(max == g)
                d_dbl[i] = 120 + 60 * (b - r) / dif;
            else
                d_dbl[i] = 240 + 60 * (r - g) / dif;
            d_dbl[i] = d_dbl[i] < 0 ? 360 + d_dbl[i] : d_dbl[i];

            if(0 != max)
                d_dbl[i + 1] = dif / max;
            else
                d_dbl[i + 1] = 0;

            d_dbl[i + 2] = max;
        }

        return scount;
    }

    bz_errno = BZ_FMT_CVRT_TYPE_NO_SUPPORT_ERROR;
    return 0;
}