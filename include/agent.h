namespace AgentBased {

    struct Action {
        bool active = false;
        int timer = 60;
    };

    struct Message {
        Vec3 coordinates;
        Vec3 heading;
    };

    class Agent {
        friend class Group;

    private:

        Model model = Model();

        struct Rotation{
            float velocity = 0.0;
            Vec3 axis = Vec3({0,0,1});
        } rotation;

        Vec3
            heading = Vec3({1, 0, 0}),
            position = Vec3({1, 0, 0}),
            velocity = Vec3({0, 0, 0});

    public:

        Message response(Message msg) {

            const Vec3 alignment = msg.heading;
            const Vec3 offset = coordinates - msg.coordinates;
            const Vec3 repulsor = offset.length() < 0.5 ? offset.normalized() : Vec3({0, 0, 0});
            const Vec3 attractor = offset.normalized() * -1.0;
            const Vec3 final = alignment + attractor + repulsor;

            const float angleOffset = vecAngle(heading, final); // angleOffset reaches NaN
            Vec3 normal = crossProduct(heading, final);
            rot_v = (normal[2] >= 0.0 ? 1.0 : -1.0) * 0.5 * angleOffset; // critically damped oscillator
            heading = heading.rotate(rot_v * radians, rotaxis);
            model = model.rotate(rot_v, rotaxis);

            return Message(coordinates, heading);

        };

        Message message () {
            return Message(coordinates, heading);
        };
    };

    class Group {

        friend class Agent;

    private:

        int size;
        Vec3 center = {0, 0, 0};
        Vec3 heading = {0, 0, 0};

    public:

        Message message() {
            return Message(*center, *heading);
        };

        Message response(Message msg) {
            heading += msg.heading / size;
            center += msg.coordinates / size;
            return Message(center, heading);
    };

};

