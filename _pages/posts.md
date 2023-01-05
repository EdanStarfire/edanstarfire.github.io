---
layout: default
permalink: "/posts"
title: "Posts"
description: "Post Archive"
---

<ul>
    {% for post in site.posts %}
    <li>
        <a href="{{ post.url }}">{{ post.title }}</a>
    </li>
    {% endfor %}
</ul>