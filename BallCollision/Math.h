#pragma once

namespace Math
{

    

    float dot(const sf::Vector2f& lhs, const sf::Vector2f& rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }

    sf::Vector2f normalize(const sf::Vector2f& vec) {
        return vec / std::sqrt(dot(vec, vec));
    }

    sf::Vector2f normal_vector(const sf::Vector2f& begin, const sf::Vector2f& end) {
        float dx = end.x - begin.x;
        float dy = end.y - begin.y;
        return sf::Vector2f(-dy, dx);
    }
    

    bool line_intersection( const sf::Vector2f& lbegin, 
                            const sf::Vector2f& lend,
                            const sf::Vector2f& rbegin, 
                            const sf::Vector2f& rend, 
                            sf::Vector2f& result) {

        float Al = lend.y - lbegin.y;
        float Bl = lbegin.x - lend.x;
        float Cl = (Al * lbegin.x) + (Bl * lbegin.y);
        
        float Ar = rend.y - rbegin.y;
        float Br = rbegin.x - rend.x;
        float Cr = (Ar * rbegin.x) + (Br * rbegin.y);

        float det = Al * Br - Ar * Bl;
        if (det != 0) {
            result = sf::Vector2f((Br * Cl - Bl * Cr) / det, (Al * Cr - Ar * Cl) / det);
            return true;
        }

        return false;
    }

    sf::Vector2f closest_point(const sf::Vector2f& begin, const sf::Vector2f& end, sf::Vector2f dot) {
        auto ratio = Math::dot(dot - begin, end - begin) / Math::dot(end - begin, end - begin);
        if (ratio <= 0)
            return begin;
        else if (ratio >= 1)
            return end;
        return begin + (end - begin) * ratio;
    }

    template<typename T, unsigned size>
    class MiddleAverageFilter {
    private:
        T data[size] = {};
        unsigned id = 0u;
        T sum = 0;
    
    public:
        void push(const T& value) {
            data[id] = value;
            id = (id + 1) % size;
        }

        T getAverage() const {
            T sum = 0;
            for (auto& i : data)	
                sum += i;
            return sum / size;
        }
    };
}
