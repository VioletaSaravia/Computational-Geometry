    // Arena test(20);

    // i32* bla = test.Alloc<i32>(2);
    // bla[0]   = 99;
    // bla[1]   = 2;

    // std::vector<std::function<i32()>> what{[]() -> i32 { return 1; },
    //                                        []() -> i32 { return 2; },
    //                                        []() -> i32 {
    //                                            std::vector<i32> big{3, 4, 6, 349234, 1239123};
    //                                            big.push_back(123123);
    //                                            std::cout << big[4] << "\n";
    //                                            std::vector<v2> also_big{v2{2, 3}};
    //                                            also_big.push_back(v2{99, 123});
    //                                            i64 even_bigger[40];
    //                                            for (usize i = 0; i < 40; i++)
    //                                                even_bigger[i] = i * 4;
    //                                            for (auto&& i : even_bigger) std::cout << i << "\n";

    //                                            return 3;
    //                                        },
    //                                        []() -> i32 { return 4; },
    //                                        std::bind([](i32 curry_me) { return curry_me + 3; }, 2)};
    // for (auto&& f : what)
    //     if (f)
    //         std::cout << f() << "\n";