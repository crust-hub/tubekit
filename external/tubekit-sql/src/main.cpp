#include <iostream>
#include <memory>

#include "query.h"
#include "connection.h"
#include "sql.h"
#include "pool.h"

using namespace tubekit::sql;
using std::shared_ptr;

int main(int argc, char **argv)
{
    pool m_pool;
    m_pool.init(10, "127.0.0.1", "root", "", "");
    {
        shared_ptr<connection> conn = m_pool.get();
        query m_query(conn);
        m_query.select("SELECT * FROM DbMailData");
        m_pool.back(conn);
    }
    {
        shared_ptr<connection> conn = m_pool.get();
        sql<1> m_sql(conn, "SELECT ID1,ID2,ID3,BlobData FROM DbRowData WHERE ID1 > ?");

        class DbRowData
        {
        public:
            DbRowData() : BlobDataSize(16777215), BlobData(new char[BlobDataSize])
            {
            }
            ~DbRowData()
            {
                if (BlobData)
                {
                    delete[] BlobData;
                }
            }
            int64_t ID1;
            int32_t ID2;
            int64_t ID3;
            const unsigned int BlobDataSize;
            char *BlobData{nullptr};
        };

        DbRowData Row;

        int64_t MinID1 = 0;
        m_sql.set_bind(0, &MinID1, sizeof(MinID1), value_type::LONGLONG);

        tubekit::sql::result<4> m_result;
        m_result.set_bind(0, &Row.ID1, sizeof(Row.ID1), value_type::LONGLONG);
        m_result.set_bind(1, &Row.ID2, sizeof(Row.ID2), value_type::LONG);
        m_result.set_bind(2, &Row.ID3, sizeof(Row.ID3), value_type::LONGLONG);
        m_result.set_bind(3, Row.BlobData, Row.BlobDataSize, value_type::MEDIUM_BLOB);

        if (m_sql.execute(m_result))
        {
            while (m_sql.fetch())
            {
                if (!m_result.is_null[0] && !m_result.is_null[1] && !m_result.is_null[2] && !m_result.is_null[3])
                {
                    std::cout << "filed len " << m_result.out_length[0] << "," << m_result.out_length[1] << "," << m_result.out_length[2] << "," << m_result.out_length[3] << std::endl;
                    std::cout << "ID1 " << Row.ID1 << " ID2 " << Row.ID2 << " ID3 " << Row.ID3 << " BlobData " << std::string(Row.BlobData) << std::endl;
                }
                else
                {
                    std::cout << "have null" << std::endl;
                }
            }
        }
        m_pool.back(conn);
    }
    return 0;
}
