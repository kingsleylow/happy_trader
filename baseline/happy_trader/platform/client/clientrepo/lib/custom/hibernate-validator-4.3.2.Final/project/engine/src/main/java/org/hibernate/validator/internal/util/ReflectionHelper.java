/*
* JBoss, Home of Professional Open Source
* Copyright 2010, Red Hat, Inc. and/or its affiliates, and individual contributors
* by the @authors tag. See the copyright.txt in the distribution for a
* full listing of individual contributors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,  
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
package org.hibernate.validator.internal.util;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.lang.reflect.TypeVariable;
import java.lang.reflect.WildcardType;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.hibernate.validator.internal.util.logging.Log;
import org.hibernate.validator.internal.util.logging.LoggerFactory;

import static org.hibernate.validator.internal.util.CollectionHelper.newHashMap;

/**
 * Some reflection utility methods. Where necessary calls will be performed as {@code PrivilegedAction} which is necessary
 * for situations where a security manager is in place.
 *
 * @author Hardy Ferentschik
 * @author Gunnar Morling
 * @author Kevin Pollet <kevin.pollet@serli.com> (C) 2011 SERLI
 */
public final class ReflectionHelper {

	private static final Log log = LoggerFactory.make();

	private static final String PROPERTY_ACCESSOR_PREFIX_GET = "get";
	private static final String PROPERTY_ACCESSOR_PREFIX_IS = "is";
	private static final String PROPERTY_ACCESSOR_PREFIX_HAS = "has";
	public static final String[] PROPERTY_ACCESSOR_PREFIXES = {
			PROPERTY_ACCESSOR_PREFIX_GET,
			PROPERTY_ACCESSOR_PREFIX_IS,
			PROPERTY_ACCESSOR_PREFIX_HAS
	};

	private static final Map<Class<?>, Class<?>> PRIMITIVE_TO_WRAPPER_TYPES;

	static {
		Map<Class<?>, Class<?>> temp = newHashMap( 9 );

		temp.put( boolean.class, Boolean.class );
		temp.put( char.class, Character.class );
		temp.put( double.class, Double.class );
		temp.put( float.class, Float.class );
		temp.put( long.class, Long.class );
		temp.put( int.class, Integer.class );
		temp.put( short.class, Short.class );
		temp.put( byte.class, Byte.class );
		temp.put( Void.TYPE, Void.TYPE );

		PRIMITIVE_TO_WRAPPER_TYPES = Collections.unmodifiableMap( temp );
	}

	/**
	 * Private constructor in order to avoid instantiation.
	 */
	private ReflectionHelper() {
	}

	/**
	 * Returns the JavaBeans property name of the given member.
	 * <p>
	 * For fields, the field name will be returned. For getter methods, the
	 * decapitalized property name will be returned, with the "get", "is" or "has"
	 * prefix stripped off. Getter methods are methods
	 * </p>
	 * <ul>
	 * <li>whose name start with "get" and who have a return type but no parameter
	 * or</li>
	 * <li>whose name starts with "is" and who have no parameter and return
	 * {@code boolean} or</li>
	 * <li>whose name starts with "has" and who have no parameter and return
	 * {@code boolean} (HV-specific, not mandated by JavaBeans spec).</li>
	 * </ul>
	 *
	 * @param member The member for which to get the property name.
	 *
	 * @return The property name for the given member or {@code null} if the
	 *         member is neither a field nor a getter method according to the
	 *         JavaBeans standard.
	 */
	public static String getPropertyName(Member member) {
		String name = null;

		if ( member instanceof Field ) {
			name = member.getName();
		}

		if ( member instanceof Method ) {
			String methodName = member.getName();
			for ( String prefix : PROPERTY_ACCESSOR_PREFIXES ) {
				if ( methodName.startsWith( prefix ) ) {
					name = StringHelper.decapitalize( methodName.substring( prefix.length() ) );
				}
			}
		}
		return name;
	}

	/**
	 * Checks whether the given method is a valid JavaBeans getter method,
	 * meaning its name starts with "is" or "has" and it has no parameters.
	 *
	 * @param method The method of interest.
	 *
	 * @return {@code true}, if the given method is a JavaBeans getter method,
	 *         {@code false} otherwise.
	 */
	public static boolean isGetterMethod(Method method) {
		return getPropertyName( method ) != null && method.getParameterTypes().length == 0;
	}

	/**
	 * @param member The <code>Member</code> instance for which to retrieve the type.
	 *
	 * @return Returns the <code>Type</code> of the given <code>Field</code> or <code>Method</code>.
	 *
	 * @throws IllegalArgumentException in case <code>member</code> is not a <code>Field</code> or <code>Method</code>.
	 */
	public static Type typeOf(Member member) {
		Type type;
		if ( member instanceof Field ) {
			type = ( (Field) member ).getGenericType();
		}
		else if ( member instanceof Method ) {
			type = ( (Method) member ).getGenericReturnType();
		}
		else {
			throw log.getMemberIsNeitherAFieldNorAMethodException( member );
		}
		if ( type instanceof TypeVariable ) {
			type = TypeHelper.getErasedType( type );
		}
		return type;
	}

	/**
	 * Returns the type of the parameter of the given method with the given parameter index.
	 *
	 * @param method The method of interest.
	 * @param parameterIndex The index of the parameter for which the type should be returned.
	 *
	 * @return The erased type.
	 */
	public static Type typeOf(Method method, int parameterIndex) {

		Type type = method.getGenericParameterTypes()[parameterIndex];

		if ( type instanceof TypeVariable ) {
			type = TypeHelper.getErasedType( type );
		}
		return type;
	}


	public static Object getValue(Member member, Object object) {
		Object value = null;

		if ( member instanceof Method ) {
			Method method = (Method) member;
			try {
				value = method.invoke( object );
			}
			catch ( IllegalAccessException e ) {
				throw log.getUnableToAccessMemberException( method.getName(), e );
			}
			catch ( InvocationTargetException e ) {
				throw log.getUnableToAccessMemberException( method.getName(), e );
			}
		}
		else if ( member instanceof Field ) {
			Field field = (Field) member;
			try {
				value = field.get( object );
			}
			catch ( IllegalAccessException e ) {
				throw log.getUnableToAccessMemberException( field.getName(), e );
			}
		}
		return value;
	}

	/**
	 * Determines the type of elements of an <code>Iterable</code>, array or the value of a <code>Map</code>.
	 *
	 * @param type the type to inspect
	 *
	 * @return Returns the type of elements of an <code>Iterable</code>, array or the value of a <code>Map</code>. <code>
	 *         null</code> is returned in case the type is not indexable (in the context of JSR 303).
	 */
	public static Type getIndexedType(Type type) {
		Type indexedType = null;
		if ( isIterable( type ) && type instanceof ParameterizedType ) {
			ParameterizedType paramType = (ParameterizedType) type;
			indexedType = paramType.getActualTypeArguments()[0];
		}
		else if ( isMap( type ) && type instanceof ParameterizedType ) {
			ParameterizedType paramType = (ParameterizedType) type;
			indexedType = paramType.getActualTypeArguments()[1];
		}
		else if ( TypeHelper.isArray( type ) ) {
			indexedType = TypeHelper.getComponentType( type );
		}
		return indexedType;
	}

	/**
	 * @param type the type to check.
	 *
	 * @return Returns <code>true</code> if <code>type</code> is a iterable type, <code>false</code> otherwise.
	 */
	public static boolean isIterable(Type type) {
		if ( type instanceof Class && Iterable.class.isAssignableFrom( (Class<?>) type ) ) {
			return true;
		}
		if ( type instanceof ParameterizedType ) {
			return isIterable( ( (ParameterizedType) type ).getRawType() );
		}
		if ( type instanceof WildcardType ) {
			Type[] upperBounds = ( (WildcardType) type ).getUpperBounds();
			return upperBounds.length != 0 && isIterable( upperBounds[0] );
		}
		return false;
	}

	/**
	 * @param type the type to check.
	 *
	 * @return Returns <code>true</code> if <code>type</code> is implementing <code>Map</code>, <code>false</code> otherwise.
	 */
	public static boolean isMap(Type type) {
		if ( type instanceof Class && Map.class.isAssignableFrom( (Class<?>) type ) ) {
			return true;
		}
		if ( type instanceof ParameterizedType ) {
			return isMap( ( (ParameterizedType) type ).getRawType() );
		}
		if ( type instanceof WildcardType ) {
			Type[] upperBounds = ( (WildcardType) type ).getUpperBounds();
			return upperBounds.length != 0 && isMap( upperBounds[0] );
		}
		return false;
	}

	/**
	 * @param type the type to check.
	 *
	 * @return Returns <code>true</code> if <code>type</code> is implementing <code>List</code>, <code>false</code> otherwise.
	 */
	public static boolean isList(Type type) {
		if ( type instanceof Class && List.class.isAssignableFrom( (Class<?>) type ) ) {
			return true;
		}
		if ( type instanceof ParameterizedType ) {
			return isList( ( (ParameterizedType) type ).getRawType() );
		}
		if ( type instanceof WildcardType ) {
			Type[] upperBounds = ( (WildcardType) type ).getUpperBounds();
			return upperBounds.length != 0 && isList( upperBounds[0] );
		}
		return false;
	}

	/**
	 * Tries to retrieve the indexed value from the specified object.
	 *
	 * @param value The object from which to retrieve the indexed value. The object has to be non <code>null</null> and
	 * either a collection or array.
	 * @param index The index. The index does not have to be numerical. <code>value</code> could also be a map in which
	 * case the index could also be a string key.
	 *
	 * @return The indexed value or <code>null</code> if <code>value</code> is <code>null</code> or not a collection or array.
	 *         <code>null</code> is also returned in case the index does not exist.
	 */
	public static Object getIndexedValue(Object value, Integer index) {
		if ( value == null ) {
			return null;
		}

		Iterator<?> iter;
		Type type = value.getClass();
		if ( isIterable( type ) ) {
			iter = ( (Iterable<?>) value ).iterator();
		}
		else if ( TypeHelper.isArray( type ) ) {
			List<?> arrayList = Arrays.asList( (Object) value );
			iter = arrayList.iterator();
		}
		else {
			return null;
		}

		int i = 0;
		Object o;
		while ( iter.hasNext() ) {
			o = iter.next();
			if ( i == index ) {
				return o;
			}
			i++;
		}
		return null;
	}

	/**
	 * Tries to retrieve the mapped value from the specified object.
	 *
	 * @param value The object from which to retrieve the mapped value. The object has to be non {@code null} and
	 * must implement the  @{code Map} interface.
	 * @param key The map key. index.
	 *
	 * @return The mapped value or {@code null} if {@code value} is {@code null} or not implementing @{code Map}.
	 */
	public static Object getMappedValue(Object value, Object key) {
		if ( !( value instanceof Map ) ) {
			return null;
		}

		Map<?, ?> map = (Map<?, ?>) value;
		//noinspection SuspiciousMethodCalls
		return map.get( key );
	}

	/**
	 * Checks, whether the given methods have the same signature, which is the
	 * case if they have the same name, parameter count and types.
	 *
	 * @param method1 A first method.
	 * @param method2 A second method.
	 *
	 * @return True, if the methods have the same signature, false otherwise.
	 */
	public static boolean haveSameSignature(Method method1, Method method2) {

		Contracts.assertValueNotNull( method1, "method1" );
		Contracts.assertValueNotNull( method2, "method2" );

		return
				method1.getName().equals( method2.getName() ) &&
						Arrays.equals( method1.getGenericParameterTypes(), method2.getGenericParameterTypes() );
	}

	/**
	 * Returns the auto-boxed type of a primitive type.
	 *
	 * @param primitiveType the primitive type
	 *
	 * @return the auto-boxed type of a primitive type. In case {@link Void} is
	 *         passed (which is considered as primitive type by
	 *         {@link Class#isPrimitive()}), {@link Void} will be returned.
	 *
	 * @throws IllegalArgumentException in case the parameter {@code primitiveType} does not
	 * represent a primitive type.
	 */
	public static Class<?> boxedType(Class<?> primitiveType) {

		Class<?> wrapperType = PRIMITIVE_TO_WRAPPER_TYPES.get( primitiveType );

		if ( wrapperType == null ) {
			throw log.getHasToBeAPrimitiveTypeException( primitiveType.getClass() );
		}

		return wrapperType;
	}

	/**
	 * Get all superclasses and optionally interfaces recursively.
	 *
	 * @param clazz The class to start the search with.
	 * @param includeInterfaces whether or not to include interfaces
	 *
	 * @return List of all super classes and interfaces of {@code clazz}. The list contains the class itself! The empty
	 *         list is returned if {@code clazz} is {@code null}.
	 */
	public static List<Class<?>> computeClassHierarchy(Class<?> clazz, boolean includeInterfaces) {
		List<Class<?>> classes = new ArrayList<Class<?>>();
		computeClassHierarchy( clazz, classes, includeInterfaces );
		return classes;
	}

	/**
	 * Get all superclasses and interfaces recursively.
	 *
	 * @param clazz The class to start the search with
	 * @param classes List of classes to which to add all found super classes and interfaces
	 * @param includeInterfaces whether or not to include interfaces
	 */
	private static void computeClassHierarchy(Class<?> clazz, List<Class<?>> classes, boolean includeInterfaces) {
		for ( Class<?> current = clazz; current != null; current = current.getSuperclass() ) {
			if ( classes.contains( current ) ) {
				return;
			}
			classes.add( current );
			if ( includeInterfaces ) {
				for ( Class<?> currentInterface : current.getInterfaces() ) {
					computeClassHierarchy( currentInterface, classes, includeInterfaces );
				}
			}
		}
	}

	/**
	 * Get all interfaces a class directly implements.
	 *
	 * @param clazz The class for which to find the interfaces
	 *
	 * @return Set of all interfaces {@code clazz} implements. The empty list is returned if {@code clazz} does not
	 *         implement any interfaces or {@code clazz} is {@code null}
	 */
	public static Set<Class<?>> computeAllImplementedInterfaces(Class<?> clazz) {
		Set<Class<?>> classes = new HashSet<Class<?>>();
		computeAllImplementedInterfaces( clazz, classes );
		return classes;
	}

	private static void computeAllImplementedInterfaces(Class<?> clazz, Set<Class<?>> classes) {
		if ( clazz == null ) {
			return;
		}
		for ( Class<?> currentInterface : clazz.getInterfaces() ) {
			classes.add( currentInterface );
			computeAllImplementedInterfaces( currentInterface, classes );
		}
	}
}
