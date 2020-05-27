#include "Ring.hpp"

Ring::Ring()
{
}

Ring::~Ring()
{
}



int Ring::createShares(ZZ secret, ZZ *secret_shares, ZZ *mac_shares)
{
    ZZ sum;
    ZZ mac(secret);
    ZZ sum_mac;
    sum = 0;
    sum_mac = 0;
    mac = mac * GLOBAL_MAC_KEY_RING;
    for (int i = 1; i < NUM_SERVERS; i++)
    {
        secret_shares[i] = RandomBits_ZZ(K);
        sum = sum + secret_shares[i];

        //MAC
        if (mac_shares != NULL)
        {
            mac_shares[i] = RandomBits_ZZ(K);
            sum_mac += mac_shares[i];
        }
    }
    secret_shares[0] = secret - sum;
    if (mac_shares != NULL)
        mac_shares[0] = mac - sum_mac;
    return 0;
}

int Ring::recoverSecret(ZZ *secret_shares, ZZ *mac_shares, ZZ &secret, ZZ &mac)
{
    secret = 0;
    mac = 0;
    for (int i = 0; i < NUM_SERVERS; i++)
    {
        secret += secret_shares[i];
        mac += mac_shares[i];
    }
    return 0;
}

int Ring::add(ZZ *secret_shares, ZZ &secret)
{
    secret = 0;
    for(int i = 0; i < NUM_SERVERS; i++)
    {
        secret += secret_shares[i];
    }
    trunc(secret, secret, K);
    return 0;
}

int Ring::multiplication(ZZ **data, ZZ **query, ZZ *party_share)
{
    ZZ **a = new ZZ[NUM_SERVERS];
    ZZ **b = new ZZ[NUM_SERVERS];
    ZZ **c = new ZZ[NUM_SERVERS];

    for (int i = 0; i < NUM_SERVERS; i++)
    {
        for(int j = 0; j < query_length; j++)
        {
            data[i][j] -= a[i];
            query[i][j] -= b[i];
        }
    }

    for (int i = 1; i < NUM_SERVERS; i++)
    {
        for (int j = 0; j < query_length; j++)
        {
            data[0][j] += data[i][j];
            query[0][j] += query[i][j];
        }
    }

    for(int i = 0; i < NUM_SERVERS; i++)
    {
        party_share[i] = conv<ZZ>(0);
        for(int j = 0; j < query_length; j++)
        {
            party_share[i] += c[i] + data[0][j] * b[i] + query[0][j] * a[i];
            if (i == 0)
            {
                party_share[i] += data[0][j] * query[0][j];
            }
            trunc(party_share[i], party_share[i], K);
        }
        cout<<party_share[i]<<endl;
    }
    
    return 0;
}